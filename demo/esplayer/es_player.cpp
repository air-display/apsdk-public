
#include "es_player.h"

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

es_player::es_player() {}

es_player::~es_player() {}

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
  m_window_thread = std::make_shared<std::thread>([this]() { video_renderer_window_loop(); });
}

void es_player::close_video_renderer() {
  if (m_window_thread) {
    SDL_Event event;
    event.type = SDL_QUIT;
    SDL_PushEvent(&event);
    m_window_thread->join();
    m_window_thread.reset();
  }
}

bool es_player::feed_video(uint8_t *data, int len) {
  AVPacket packet;
  av_init_packet(&packet);
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
    // enqueue the frame for rendering
    m_video_render_frm_queue.enqueue(frm);
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

void es_player::open_audio_renderer() {}

void es_player::close_audio_renderer() {}

bool es_player::feed_audio(uint8_t *data, int len) {
  AVPacket packet;
  av_init_packet(&packet);
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
    m_audio_frm_pool.recycle(frm);
    return true;

    // enqueue the frame for rendering
    m_audio_render_frm_queue.enqueue(frm);
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

#define DEFAULT_WIN_WIDTH 640
#define DEFAULT_WIN_HEIGHT 480

bool es_player::create_video_renderer_resource() {
  // create a window
  auto sdl_window = SDL_CreateWindow("ESPlayer",
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     DEFAULT_WIN_WIDTH,
                                     DEFAULT_WIN_HEIGHT,
                                     SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
  if (sdl_window) {
    // create the renderer
    auto sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
    if (sdl_renderer) {
      // create texture
      auto sdl_texture = SDL_CreateTexture(
          sdl_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, DEFAULT_WIN_WIDTH, DEFAULT_WIN_HEIGHT);
      if (sdl_texture) {
        // create AVFrame to receive the scaled frame data
        auto yuv_frm = av_frame_alloc();
        if (yuv_frm) {
          yuv_frm->format = AV_PIX_FMT_YUV420P;
          yuv_frm->width = DEFAULT_WIN_WIDTH;
          yuv_frm->height = DEFAULT_WIN_HEIGHT;
          if (0 == av_frame_get_buffer(yuv_frm, 0)) {
            // reset the render
            destroy_video_renderer_resource();

            // update
            m_yuv_frame = yuv_frm;
            m_sdl_texture = sdl_texture;
            m_sdl_renderer = sdl_renderer;
            m_sdl_window = sdl_window;

            if (m_worker_thread) {
              m_worker_thread->join();
              m_worker_thread.reset();
            }
            m_worker_thread = std::make_shared<std::thread>([this]() { video_renderer_worker_loop(); });

            return true;
          }
          SDL_Log("Could not create frame buffer: %s", SDL_GetError());
          av_frame_free(&yuv_frm);
        }
        SDL_Log("Could not create frame buffer: %s", SDL_GetError());
        SDL_DestroyTexture(sdl_texture);
      }
      SDL_Log("Could not create a texture: %s", SDL_GetError());
      SDL_DestroyRenderer(sdl_renderer);
    }
    SDL_Log("Could not create a renderer: %s", SDL_GetError());
    SDL_DestroyWindow(sdl_window);
  }
  SDL_Log("Could not create a window: %s", SDL_GetError());

  return false;
}

void es_player::destroy_video_renderer_resource() {

  if (m_worker_thread) {
    m_worker_thread->join();
    m_worker_thread.reset();
  }

  if (m_yuv_frame)
    av_frame_free(&m_yuv_frame);

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

void es_player::update_video_render_resource() {
  std::lock_guard<std::mutex> l(m_video_resource_update_lock);
  if (m_sdl_texture) {
    SDL_DestroyTexture(m_sdl_texture);
    m_sdl_texture = nullptr;
  }

  if (m_sdl_renderer) {
    SDL_DestroyRenderer(m_sdl_renderer);
    m_sdl_renderer = nullptr;
  }

  m_sdl_renderer = SDL_CreateRenderer(m_sdl_window, -1, SDL_RENDERER_ACCELERATED);
  if (!m_sdl_renderer) {
    SDL_Log("Failed to create renderer: %s", SDL_GetError());
    return;
  }

  // create new texture
  int width = DEFAULT_WIN_WIDTH;
  int height = DEFAULT_WIN_HEIGHT;

  SDL_GetWindowSize(m_sdl_window, &width, &height);
  m_sdl_texture = SDL_CreateTexture(m_sdl_renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
  if (!m_sdl_texture) {
    SDL_Log("Failed to create texture: %s", SDL_GetError());
    return;
  }
}

void es_player::scale_frame(const AVFrame *frame) {
  // get latest windows size;
  int texture_width = 0;
  int texture_height = 0;
  SDL_QueryTexture(m_sdl_texture, nullptr, nullptr, &texture_width, &texture_height);

  // check whether the src size or dst size has changed or not
  if (m_last_source_height != frame->height      //
      || m_last_source_width != frame->width     //
      || m_last_texture_widht != texture_width   //
      || m_last_texture_height != texture_height //
  ) {
    // create new frame buffer
    m_yuv_frame = av_frame_alloc();
    m_yuv_frame->format = AV_PIX_FMT_YUV420P;
    m_yuv_frame->width = texture_width;
    m_yuv_frame->height = texture_height;
    av_frame_get_buffer(m_yuv_frame, 0);

    // reset
    if (m_sws_ctx) {
      sws_freeContext(m_sws_ctx);
      m_sws_ctx = nullptr;
    }

    // create scale context
    m_sws_ctx = sws_getContext(frame->width,                       // src width
                               frame->height,                      // src height
                               (AVPixelFormat)frame->format,       // src format
                               m_yuv_frame->width,                 // dst width
                               m_yuv_frame->height,                // dst height
                               (AVPixelFormat)m_yuv_frame->format, // dst format
                               SWS_BICUBIC,                        // flags
                               nullptr,                            // src filter
                               nullptr,                            // dst filter
                               nullptr                             // param
    );

    // update last status
    m_last_source_width = frame->width;
    m_last_source_height = frame->height;

    m_last_texture_widht = m_yuv_frame->width;
    m_last_texture_height = m_yuv_frame->height;
  }

  // perform scaling
  sws_scale(m_sws_ctx,                           // sws context
            (const uint8_t *const *)frame->data, // src slice
            frame->linesize,                     // src stride
            0,                                   // src slice y
            frame->height,                       // src slice height
            m_yuv_frame->data,                   // dst planes
            m_yuv_frame->linesize                // dst strides
  );
}

void es_player::present_frame() {
  int ret = 0;

  // update the texture
  ret = SDL_UpdateYUVTexture(m_sdl_texture,            // sdl texture
                             nullptr,                  // sdl rect
                             m_yuv_frame->data[0],     // y plane
                             m_yuv_frame->linesize[0], // y pitch
                             m_yuv_frame->data[1],     // u plane
                             m_yuv_frame->linesize[1], // u pitch
                             m_yuv_frame->data[2],     // v plane
                             m_yuv_frame->linesize[2]  // v pitch
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
  ret = SDL_SetRenderDrawColor(m_sdl_renderer, 0, 0, 255, 255);
  if (0 != ret) {
    SDL_Log("Failed to SDL_SetRenderDrawColor: %s", SDL_GetError());
  }

  // fill the background with the selected draw color
  ret = SDL_RenderFillRect(m_sdl_renderer, nullptr);
  if (0 != ret) {
    SDL_Log("Failed to SDL_RenderFillRect: %s", SDL_GetError());
  }

  // copy the texture to the renderer
  ret = SDL_RenderCopy(m_sdl_renderer, // sdl renderer
                       m_sdl_texture,  // sdl texture
                       nullptr,        // src rect, if NULL copy texture
                       nullptr         // dst rect
  );
  if (0 != ret) {
    SDL_Log("Failed to SDL_RenderCopy: %s", SDL_GetError());
  }

  // present the frame on target
  SDL_RenderPresent(m_sdl_renderer);
}

void es_player::video_renderer_window_loop() {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    return;
  }

  create_video_renderer_resource();

  Uint32 window_id = SDL_GetWindowID(m_sdl_window);

  // enter the rendering loop
  while (true) {
    // handle event first
    SDL_Event event;

    SDL_PumpEvents();
    if (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)) {
      // if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }

      // update render resource
      if (event.type == SDL_WINDOWEVENT && event.window.windowID == window_id &&
          event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        update_video_render_resource();
      }
    }
  }

  // destroy
  destroy_video_renderer_resource();

  SDL_Quit();
}

void es_player::video_renderer_worker_loop() {
  AVFrame *frm = nullptr;
  while (frm = m_video_render_frm_queue.dequeue()) {
    // process the video frame
    if (frm) {
      std::lock_guard<std::mutex> l(m_video_resource_update_lock);

      // scale frame
      scale_frame(frm);

      // recycle the frame
      m_video_frm_pool.recycle(frm);

      // present the frame
      present_frame();
    }
  }
}

} // namespace esp
