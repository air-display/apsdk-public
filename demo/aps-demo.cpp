// aps-demo.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "../src/ap_config.h"
#include "../src/ap_server.h"
#include "../src/ap_session.h"
#include "../src/ap_types.h"
#include "esplayer/es_player.h"

#define LOG() std::cout
#define LOGV() std::cout
#define LOGD() std::cout
#define LOGI() std::cout
#define LOGW() std::cout
#define LOGE() std::cout
#define LOGF() std::cout

class airplay_mirror_handler : public aps::ap_mirror_session_handler {
private:
  esp::es_player es_player_;

public:
  airplay_mirror_handler(){};

  ~airplay_mirror_handler(){};

  virtual void on_mirror_stream_started() override {
    LOGI() << "on_mirror_stream_started" << std::endl;

    es_player_.open_video_renderer();
  }

  virtual void on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p) override {
    LOGI() << "on_mirror_stream_codec: " << std::endl;

    es_player_.open_video_decoder(p->payload, p->payload_size);
  }

  virtual void on_mirror_stream_data(const aps::sms_video_data_packet_t *p) override {
    LOGI() << "on_mirror_stream_data payload_size: " << p->payload_size << ", timestamp: " << p->timestamp << std::endl;

    es_player_.feed_video(const_cast<uint8_t *>(p->payload), p->payload_size);
  }

  virtual void on_mirror_stream_stopped() override {
    LOGI() << "on_mirror_stream_stopped" << std::endl;

    es_player_.close_video_renderer();
    es_player_.close_video_decoder();
  }

  virtual void on_audio_set_volume(const float ratio, const float volume) override {
    LOGI() << "on_audio_set_volume: " << ratio << ", value: " << volume << std::endl;
  }

  virtual void on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current,
                                     const uint64_t end) override {
    LOGI() << "on_audio_set_progress: " << ratio << ", start: " << start << ", current: " << current << ", end: " << end
           << std::endl;
  }

  virtual void on_audio_set_cover(const std::string format, const void *data, const uint32_t length) override {
    LOGI() << "on_audio_set_cover: " << format << std::endl;
  }

  virtual void on_audio_set_meta_data(const void *data, const uint32_t length) override {
    LOGI() << "on_audio_set_meta_data: " << data << std::endl;
  }

  virtual void on_audio_stream_started(const aps::audio_data_format_t format) override {
    LOGI() << "on_audio_stream_started: " << format << std::endl;
    uint8_t asc[] = {0xF8, 0xE8, 0x50, 0x00};

    es_player_.open_audio_decoder(asc, sizeof(asc));
    es_player_.open_audio_renderer();
  }

  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length) override {
    LOGV() << "on_audio_stream_data: " << payload_length << ", timestamp: " << p->timestamp << std::endl;

    es_player_.feed_audio(const_cast<uint8_t *>(p->payload), payload_length);
  }

  virtual void on_audio_stream_stopped() override {
    LOGI() << "on_audio_stream_stopped" << std::endl;

    es_player_.close_audio_renderer();
    es_player_.close_audio_decoder();
  }

  virtual void on_mirror_stream_heartbeat() override { LOGD() << "on_mirror_stream_heartbeat" << std::endl; }
};

class airplay_video_handler : public aps::ap_video_session_handler {
public:
  uint64_t session_;
  airplay_video_handler(uint64_t sid) : session_(sid){};
  ~airplay_video_handler(){};

  virtual void on_video_play(const uint64_t session_id, const std::string &location, const float start_pos) override {
    LOGI() << "on_video_play: " << location << ", session: " << session_id << std::endl;
    session_ = session_id;
  }

  virtual void on_video_scrub(const uint64_t session_id, const float position) override {
    LOGI() << "on_video_scrub: " << position << std::endl;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_ << std::endl;
    }
  }

  virtual void on_video_rate(const uint64_t session_id, const float value) override {
    LOGI() << "on_video_rate: " << value << std::endl;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_ << std::endl;
    }
  }

  virtual void on_video_stop(const uint64_t session_id) override {
    LOGI() << "on_video_stop: " << std::endl;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_ << std::endl;
    }
  }

  virtual void on_acquire_playback_info(const uint64_t session_id, aps::playback_info_t &playback_info) override {
    LOGV() << "on_acquire_playback_info: " << std::endl;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_ << std::endl;
    }
  }
};

class airplay_handler : public aps::ap_handler {
public:
  airplay_handler();
  ~airplay_handler();

  virtual void on_session_begin(aps::ap_session_ptr session) override {
    if (aps::video_session == session->get_session_type()) {
      on_video_session_begin(session);
    } else if (aps::mirror_session == session->get_session_type()) {
      on_mirror_session_begin(session);
    }
  }

  virtual void on_session_end(const uint64_t session_id) override {
    LOGI() << "-------------------on_session_end: " << session_id << std::endl;
  }

  void on_mirror_session_begin(aps::ap_session_ptr session) {
    uint64_t sid = session->get_session_id();
    LOGI() << "###################on_mirror_session_begin: " << sid << std::endl;
    aps::ap_mirror_session_handler_ptr mirror_handler = std::make_shared<airplay_mirror_handler>();
    session->set_mirror_handler(mirror_handler);
  }

  void on_video_session_begin(aps::ap_session_ptr session) {
    uint64_t sid = session->get_session_id();
    LOGI() << "+++++++++++++++++++on_video_session_begin: " << sid << std::endl;
    aps::ap_video_session_handler_ptr video_handler = std::make_shared<airplay_video_handler>(sid);
    session->set_video_handler(video_handler);
  }
};

airplay_handler::airplay_handler() {}

airplay_handler::~airplay_handler() {}

int main() {
  // logger::init_logger(false, log_level::LL_INFO);
  aps::ap_server_ptr server = std::make_shared<aps::ap_server>();
  aps::ap_handler_ptr handler = std::make_shared<airplay_handler>();
  aps::ap_config_ptr config = aps::ap_config::default_instance();
  // config->features(0x527FF18D);  //0x527FFFF7
  config->macAddress("AA:BB:CC:DD:EE:FF");
  config->name("APS[" + config->macAddress() + "]");
  server->set_config(config);
  server->set_handler(handler);
  LOGI() << "AP Server is starting...." << std::endl;
  server->start();
  LOGI() << "AP Server started...." << std::endl;
  (void)(getchar());
  server->stop();
  return 0;
}
