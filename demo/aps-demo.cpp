// aps-demo.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include "../src/ap_config.h"
#include "../src/ap_server.h"
#include "../src/utils/logger.h"

class ap_player : public aps::ap_handler {
 public:
  ap_player();
  ~ap_player();

  virtual void on_mirror_stream_started(const std::string &session) override {
    LOGI() << "on_mirror_stream_started";
  }

  virtual void on_mirror_stream_stopped(const std::string &session) override {
    LOGI() << "on_mirror_stream_stopped";
  }

  virtual void on_audio_set_volume(const std::string &session, const float ratio,
                                   const float volume) override {
    LOGI() << "on_audio_set_volume: " << ratio << ", value: " << volume;
  }

  virtual void on_audio_set_progress(
      const std::string &session, const float ratio, const uint64_t start,
                                     const uint64_t current,
                                     const uint64_t end) override {
    LOGI() << "on_audio_set_progress: " << ratio << ", start: " << start
           << ", current: " << current << ", end: " << end;
  }

  virtual void on_audio_set_cover(
      const std::string &session, const std::string format, const void *data,
                                  const uint32_t length) override {
    LOGI() << "on_audio_set_cover: " << format;
  }

  virtual void on_audio_set_meta_data(
      const std::string &session, const void *data,
                                      const uint32_t length) override {
    LOGI() << "on_audio_set_meta_data: " << data;
  }

  virtual void on_audio_stream_started(
      const std::string &session, const
          aps::audio_data_format_t format) override {
    LOGI() << "on_audio_stream_started: ";
  }

  virtual void on_audio_stream_stopped(const std::string &session) override {
    LOGI() << "on_audio_stream_stopped";
  }

  virtual void on_video_play(const std::string &session, const
                                 std::string &location,
                             const float start_pos) override {
    LOGI() << "on_video_play: " << location;
  }

  virtual void on_video_scrub(
      const std::string &session, const float position) override {
    LOGI() << "on_video_scrub: " << position;
  }

  virtual void on_video_rate(
      const std::string &session, const float value) override {
    LOGI() << "on_video_rate: " << value;
  }

  virtual void on_video_stop(const std::string &session) override {
    LOGI() << "on_video_stop: ";
  }

  virtual void on_acquire_playback_info(
      const std::string &session,
      playback_info_t &playback_info) override {
    LOGI() << "on_acquire_playback_info: ";
  }

  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                    const uint32_t payload_length) override {
    LOGV() << "on_audio_stream_data: " << payload_length;
  }

  virtual void on_mirror_stream_data(
      const aps::sms_video_data_packet_t *p) override {
    uint8_t *cursor = (uint8_t *)(p->payload);
    uint32_t frame_size = 0;
    frame_size <<= 8;
    frame_size |= cursor[0];
    frame_size <<= 8;
    frame_size |= cursor[1];
    frame_size <<= 8;
    frame_size |= cursor[2];
    frame_size <<= 8;
    frame_size |= cursor[3];
    LOGD() << "on_mirror_stream_data, frame size: " << frame_size;
  }

  virtual void on_mirror_stream_codec(
      const aps::sms_video_codec_packet_t *p) override {
    LOGI() << "on_mirror_stream_codec: ";
  }
};

ap_player::ap_player() {}

ap_player::~ap_player() {}

int main() {
  aps::ap_server_ptr server = std::make_shared<aps::ap_server>();
  aps::ap_handler_ptr player = std::make_shared<ap_player>();
  aps::ap_config_ptr config = aps::ap_config::default_instance();
  config->macAddress("AA:BB:CC:DD:EE:FF");
  config->name("APS[" + config->macAddress() +"]");
  server->set_config(config);
  server->set_handler(player);
  LOGI() << "AP Server is starting....";
  server->start();
  LOGI() << "AP Server started....";
  getchar();
  server->stop();
  return 0;
}
