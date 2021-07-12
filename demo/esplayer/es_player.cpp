
#include "es_player.h"

#include <stdexcept>

namespace esp {

es_frame_pool::es_frame_pool(int capacity /*= 16*/) : m_capacity(capacity) {
  std::lock_guard<std::mutex> l(m_mtx);
  for (int n = 0; n < m_capacity; n++) {
    auto f = av_frame_alloc();
    m_q.push_back(f);
  }
}

es_frame_pool::~es_frame_pool() {
  std::lock_guard<std::mutex> l(m_mtx);
  for (auto f : m_q)
    av_frame_free(&f);

  m_q.clear();
}

AVFrame *es_frame_pool::acquire() {
  std::unique_lock<std::mutex> l(m_mtx);

  // empty, no fame available, wait
  while (m_q.empty())
    m_cv.wait(l);

  if (m_q.empty())
    return nullptr;

  AVFrame *f = m_q.front();
  m_q.pop_front();
  return f;
}

void es_frame_pool::recycle(AVFrame *frm) {
  std::lock_guard<std::mutex> l(m_mtx);
  m_q.push_back(frm);
  m_cv.notify_all();
}

// //////////////////////////////////////////////////////////////////////////

es_frame_queue::es_frame_queue() { m_abandon = false; }

es_frame_queue::~es_frame_queue() {
  std::unique_lock<std::mutex> l(m_mtx);
  m_abandon = true;
  for (auto f : m_q)
    av_frame_free(&f);

  m_cv.notify_all();
  m_q.clear();
}

AVFrame *es_frame_queue::dequeue() {
  std::unique_lock<std::mutex> l(m_mtx);
  while (!m_abandon && m_q.empty())
    m_cv.wait(l);

  if (m_abandon) {
    m_abandon = false;
    return nullptr;
  }

  auto f = m_q.front();
  m_q.pop_front();

  return f;
}

AVFrame *es_frame_queue::try_dequeue() {
  AVFrame *f = nullptr;
  if (m_mtx.try_lock()) {
    if (m_q.size()) {
      f = m_q.front();
      m_q.pop_front();
    }
    m_mtx.unlock();
  }
  return f;
}

void es_frame_queue::enqueue(AVFrame *frm) {
  std::unique_lock<std::mutex> l(m_mtx);
  m_q.push_back(frm);
  m_cv.notify_all();
}

// //////////////////////////////////////////////////////////////////////////

es_player::es_player() : m_video_frm_pool(16), m_audio_frm_pool(2) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    throw std::runtime_error("failed to init SDL");
  }
}

es_player::~es_player() {
  //
  SDL_Quit();
}

bool es_player::open_video_decoder(const uint8_t *config, int len) {
  //
  return create_video_decoder_resource(config, len);
}

void es_player::close_video_decoder() {
  //
  destroy_video_decoder_resource();
}

void es_player::open_video_renderer() {
  close_video_renderer();
  m_video_rendering_thread = std::make_shared<std::thread>([this]() { video_rendering_loop(); });
}

void es_player::close_video_renderer() {
  if (m_video_rendering_thread) {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    m_video_rendering_thread->join();
    m_video_rendering_thread.reset();
  }
}

bool es_player::feed_video(uint8_t *data, int len) {
  AVPacket packet;
  av_new_packet(&packet, 0);
  packet.data = data;
  packet.size = len;

  // send the packet to decoder
  int ret = avcodec_send_packet(m_video_codec_ctx, &packet);
  if (ret < 0) {
    return false;
  }

  // acquire a frame from frame pool
  AVFrame *frm = m_video_frm_pool.acquire();

  // try to receive the frame from decoder
  ret = avcodec_receive_frame(m_video_codec_ctx, frm);
  if (ret >= 0) {
    render_video_frame(frm);
    return true;
  }

  // no frame decoded
  m_video_frm_pool.recycle(frm);
  if (AVERROR(EAGAIN) == ret || AVERROR_EOF == ret) {
    return true;
  }

  return false;
}

bool es_player::open_audio_decoder(const uint8_t *config, int len) {
  //
  return create_audio_decoder_resource(config, len);
}

void es_player::close_audio_decoder() {
  //
  destroy_audio_decoder_resource();
}

void es_player::open_audio_renderer(int frequency) {
  close_audio_renderer();

  SDL_AudioSpec requiredSpec;
  SDL_AudioSpec obtainedSpec;
  SDL_zero(requiredSpec);
  SDL_zero(obtainedSpec);
  requiredSpec.freq = frequency;
  m_sdl_audio_device = SDL_OpenAudioDevice(nullptr, 0, &requiredSpec, &obtainedSpec, 0);
  if (!m_sdl_audio_device) {
    SDL_Log("Failed to SDL_OpenAudioDevice %s\n", SDL_GetError());
    return;
  }

  SDL_PauseAudioDevice(m_sdl_audio_device, 0);
}

void es_player::close_audio_renderer() {
  if (m_sdl_audio_device) {
    SDL_PauseAudioDevice(m_sdl_audio_device, 1);
    SDL_CloseAudioDevice(m_sdl_audio_device);
    m_sdl_audio_device = 0;
  }
}

bool es_player::feed_audio(uint8_t *data, int len) {
  AVPacket packet;
  av_new_packet(&packet, 0);
  packet.data = data;
  packet.size = len;

  // send the packet to decoder
  int ret = avcodec_send_packet(m_audio_codec_ctx, &packet);
  if (ret < 0) {
    return false;
  }

  // acquire a frame from frame pool
  AVFrame *frm = m_audio_frm_pool.acquire();

  // try to receive the frame from decoder
  ret = avcodec_receive_frame(m_audio_codec_ctx, frm);
  if (ret >= 0) {
    render_audio_frame(frm);
    return true;
  }

  // no frame decoded
  m_audio_frm_pool.recycle(frm);
  if (AVERROR(EAGAIN) == ret || AVERROR_EOF == ret) {
    return true;
  }

  return false;
}

bool es_player::create_video_decoder_resource(const uint8_t *config, int len) {
  AVCodec *codec = avcodec_find_decoder(AVCodecID::AV_CODEC_ID_H264);
  if (codec) {
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx) {
      codec_ctx->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
      uint8_t *extradata_buf = (uint8_t *)av_malloc(len + AV_INPUT_BUFFER_PADDING_SIZE);
      memcpy(extradata_buf, config, len);
      codec_ctx->extradata = extradata_buf;
      codec_ctx->extradata_size = len;
      if (avcodec_open2(codec_ctx, codec, nullptr) >= 0) {
        // reset
        destroy_video_decoder_resource();

        // update
        m_video_codec_ctx = codec_ctx;
        return true;
      }

      // failed to open the codec context
      av_free(extradata_buf);
      avcodec_free_context(&codec_ctx);
    }
  }

  return false;
}

void es_player::destroy_video_decoder_resource() {
  if (m_video_codec_ctx) {
    avcodec_free_context(&m_video_codec_ctx);
  }
}

void es_player::render_video_frame(AVFrame *frm) {
  // enqueue the frame for rendering
  m_video_render_frm_queue.enqueue(frm);

  // send the SDL event
  SDL_Event evt;
  evt.type = SDL_USEREVENT;
  evt.user.code = 0;
  SDL_PushEvent(&evt);
}

bool es_player::create_audio_decoder_resource(const uint8_t *config, int len) {
  AVCodec *codec = avcodec_find_decoder_by_name("libfdk_aac");
  if (codec) {
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (codec_ctx) {
      // Initialize codecCtx
      codec_ctx->codec_type = AVMediaType::AVMEDIA_TYPE_AUDIO;
      uint8_t *extradata_buf = (uint8_t *)av_malloc(len + AV_INPUT_BUFFER_PADDING_SIZE);
      memcpy(extradata_buf, config, len);
      codec_ctx->extradata = extradata_buf;
      codec_ctx->extradata_size = len;
      if (avcodec_open2(codec_ctx, codec, nullptr) >= 0) {
        // reset
        destroy_audio_decoder_resource();

        // update
        m_audio_codec_ctx = codec_ctx;
        return true;
      }

      // clean
      avcodec_free_context(&codec_ctx);
    }
  }

  return false;
}

void es_player::destroy_audio_decoder_resource() {
  if (m_audio_codec_ctx) {
    avcodec_free_context(&m_audio_codec_ctx);
  }
}

void es_player::render_audio_frame(AVFrame *frm) {
  int ret = 0;
  int size = 0;
  int bufsize =
      av_samples_get_buffer_size(&size, m_audio_codec_ctx->channels, frm->nb_samples, (AVSampleFormat)frm->format, 0);
  bool isplanar = av_sample_fmt_is_planar((AVSampleFormat)frm->format) == 1;

  // queue the data to the device
  for (int ch = 0; ch < m_audio_codec_ctx->channels; ch++) {
    if (!isplanar) {
      ret = SDL_QueueAudio(m_sdl_audio_device, frm->data[ch], frm->linesize[ch]);
    } else {
      ret = SDL_QueueAudio(m_sdl_audio_device, frm->data[0] + size * ch, size);
    }

    if (ret < 0) {
      SDL_Log("Failed to SDL_QueueAudio %s\n", SDL_GetError());
    }
  }

  m_audio_frm_pool.recycle(frm);
}

#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480

bool es_player::create_video_renderer_resource() {
  // create a window
  auto sdl_window = SDL_CreateWindow("ESPlayer",
                                     SDL_WINDOWPOS_CENTERED,       //
                                     SDL_WINDOWPOS_CENTERED,       //
                                     DEFAULT_WIN_WIDTH,            //
                                     DEFAULT_WIN_HEIGHT,           //
                                     SDL_WINDOW_FULLSCREEN_DESKTOP //
  );
  if (sdl_window) {
    // create the renderer
    auto sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer) {
      // reset the render
      destroy_video_renderer_resource();

      // update
      m_sdl_renderer = sdl_renderer;
      m_sdl_window = sdl_window;

      return true;
    }
    SDL_Log("Could not create a renderer: %s", SDL_GetError());
    SDL_DestroyWindow(sdl_window);
  }
  SDL_Log("Could not create a window: %s", SDL_GetError());

  return false;
}

void es_player::destroy_video_renderer_resource() {
  if (m_picture_frame)
    av_frame_free(&m_picture_frame);

  if (m_sdl_texture) {
    SDL_DestroyTexture(m_sdl_texture);
    m_sdl_texture = nullptr;
  }

  if (m_sdl_renderer) {
    SDL_DestroyRenderer(m_sdl_renderer);
    m_sdl_renderer = nullptr;
  }

  if (m_sdl_window) {
    SDL_DestroyWindow(m_sdl_window);
    m_sdl_window = nullptr;
  }
}

void es_player::video_rendering_loop() {

  create_video_renderer_resource();

  Uint32 window_id = SDL_GetWindowID(m_sdl_window);

  // enter the rendering loop
  while (true) {
    // handle event first
    SDL_Event event;

    // SDL_PumpEvents();
    // if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
    if (SDL_PollEvent(&event)) {
      // update video
      if (event.type == SDL_USEREVENT && event.user.code == 0) {
        present_frame();
      }

      // update render resource
      if (event.type == SDL_WINDOWEVENT && event.window.windowID == window_id &&
          event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      }

      // if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }
    }
  }

  // destroy
  destroy_video_renderer_resource();
}

void es_player::update_video_renderer_resource(const AVFrame *frame) {
  // get latest windows size;
  int window_width = 0;
  int window_height = 0;
  SDL_GetWindowSize(m_sdl_window, &window_width, &window_height);

  // check whether the src size or dst size has changed or not
  if (m_last_source_height != frame->height    //
      || m_last_source_width != frame->width   //
      || m_last_window_width != window_width   //
      || m_last_window_height != window_height //
  ) {
    SDL_Log("===== Source size: %d x %d", frame->width, frame->height);
    // calculate the new image size
    float width_ratio = (float)window_width / frame->width;
    float height_ratio = (float)window_height / frame->height;
    int picture_width = (int)((width_ratio <= height_ratio) ? frame->width * width_ratio : frame->width * height_ratio);
    int picture_height =
        (int)((width_ratio <= height_ratio) ? frame->height * height_ratio : frame->height * height_ratio);

    // update the texture
    if (m_sdl_texture) {
      SDL_DestroyTexture(m_sdl_texture);
    }
    m_sdl_texture = SDL_CreateTexture(
        m_sdl_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, picture_width, picture_height);
    if (!m_sdl_texture) {
      SDL_Log("Failed to SDL_CreateTexture: %s", SDL_GetError());
    }

    // update picture frame buffer
    if (m_picture_frame) {
      av_frame_free(&m_picture_frame);
    }
    m_picture_frame = av_frame_alloc();
    m_picture_frame->format = AV_PIX_FMT_YUV420P;
    m_picture_frame->width = picture_width;
    m_picture_frame->height = picture_height;
    av_frame_get_buffer(m_picture_frame, 0);

    // update scale context, reset and create new one
    if (m_sws_ctx) {
      sws_freeContext(m_sws_ctx);
      m_sws_ctx = nullptr;
    }
    m_sws_ctx = sws_getContext(frame->width,                           // src width
                               frame->height,                          // src height
                               (AVPixelFormat)frame->format,           // src format
                               m_picture_frame->width,                 // dst width
                               m_picture_frame->height,                // dst height
                               (AVPixelFormat)m_picture_frame->format, // dst format
                               SWS_BICUBIC,                            // flags
                               nullptr,                                // src filter
                               nullptr,                                // dst filter
                               nullptr                                 // param
    );

    // update last status
    m_last_source_width = frame->width;
    m_last_source_height = frame->height;
    m_last_window_width = window_width;
    m_last_window_height = window_height;
  }
}

void es_player::update_video_renderer_frame(const AVFrame *frame) {
  // perform scaling
  sws_scale(m_sws_ctx,                           // sws context
            (const uint8_t *const *)frame->data, // src slice
            frame->linesize,                     // src stride
            0,                                   // src slice y
            frame->height,                       // src slice height
            m_picture_frame->data,               // dst planes
            m_picture_frame->linesize            // dst strides
  );

  // draw the image to the texture
  int ret = SDL_UpdateYUVTexture(m_sdl_texture,                // sdl texture
                                 nullptr,                      // sdl rect
                                 m_picture_frame->data[0],     // y plane
                                 m_picture_frame->linesize[0], // y pitch
                                 m_picture_frame->data[1],     // u plane
                                 m_picture_frame->linesize[1], // u pitch
                                 m_picture_frame->data[2],     // v plane
                                 m_picture_frame->linesize[2]  // v pitch
  );
  if (0 != ret) {
    SDL_Log("Failed to SDL_UpdateYUVTexture: %s", SDL_GetError());
  }

  // clear the renderer and redraw background
  ret = SDL_RenderClear(m_sdl_renderer);
  if (0 != ret) {
    SDL_Log("Failed to SDL_RenderClear: %s", SDL_GetError());
  }

  // set draw color
  ret = SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 0, 255);
  if (0 != ret) {
    SDL_Log("Failed to SDL_SetRenderDrawColor: %s", SDL_GetError());
  }

  // fill the background with the selected draw color
  ret = SDL_RenderFillRect(m_sdl_renderer, nullptr);
  if (0 != ret) {
    SDL_Log("Failed to SDL_RenderFillRect: %s", SDL_GetError());
  }

  // calculate the destination center rect
  SDL_Rect dst_rc = {
      (m_last_window_width / 2) - (m_picture_frame->width / 2),   //
      (m_last_window_height / 2) - (m_picture_frame->height / 2), //
      m_picture_frame->width,                                     //
      m_picture_frame->height                                     //
  };

  // copy the texture to the renderer
  ret = SDL_RenderCopy(m_sdl_renderer, // sdl renderer
                       m_sdl_texture,  // sdl texture
                       nullptr,        // src rect, if NULL copy texture
                       &dst_rc         // dst rect
  );
  if (0 != ret) {
    SDL_Log("Failed to SDL_RenderCopy: %s", SDL_GetError());
  }

  // present the frame on target
  SDL_RenderPresent(m_sdl_renderer);
}

void es_player::present_frame() {
  AVFrame *frm = m_video_render_frm_queue.dequeue();
  update_video_renderer_resource(frm);
  update_video_renderer_frame(frm);
  m_video_frm_pool.recycle(frm);
}
} // namespace esp
