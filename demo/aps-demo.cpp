// aps-demo.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include <strstream>
#include <fstream>
#include <asio.hpp>
#include "../src/ap_config.h"
#include "../src/ap_server.h"
#include "../src/ap_session.h"
#include "../src/utils/logger.h"
#include "../src/utils/packing.h"
#include "aac_eld.h"

class airplay_mirror_handler : public aps::ap_mirror_session_handler {
 public:
  airplay_mirror_handler() {
    time_t now = time(0);
    std::tm *local_now = localtime(&now);
    std::ostringstream oss;
    oss
      << local_now->tm_year + 1900 << "-"
      << local_now->tm_mon + 1 << "-"
      << local_now->tm_mday << "-"
      << local_now->tm_hour << "-"
      << local_now->tm_min << "-"
      << local_now->tm_sec;
    file_id_ = oss.str();
  };
  ~airplay_mirror_handler() {};

  virtual void on_mirror_stream_started() override {
    LOGI() << "on_mirror_stream_started";
    init_video_data_file();
  }

  virtual void on_mirror_stream_codec(
      const aps::sms_video_codec_packet_t *p) override {
    LOGI() << "on_mirror_stream_codec: ";
    append_avc_sequence_header(p);
  }

  virtual void on_mirror_stream_data(
      const aps::sms_video_data_packet_t *p) override {
    uint32_t frame_size = p->payload_size - sizeof(uint32_t);
    LOGV() << "on_mirror_stream_data, frame size: " << frame_size;
    append_nalu_data(p);
  }

  virtual void on_mirror_stream_stopped() override {
    LOGI() << "on_mirror_stream_stopped";
    close_video_data_file();
  }

  virtual void on_audio_set_volume(const float ratio,
                                   const float volume) override {
    LOGI() << "on_audio_set_volume: " << ratio << ", value: " << volume;
  }

  virtual void on_audio_set_progress(const float ratio, const uint64_t start,
                                     const uint64_t current,
                                     const uint64_t end) override {
    LOGI() << "on_audio_set_progress: " << ratio << ", start: " << start
           << ", current: " << current << ", end: " << end;
  }

  virtual void on_audio_set_cover(const std::string format, const void *data,
                                  const uint32_t length) override {
    LOGI() << "on_audio_set_cover: " << format;
  }

  virtual void on_audio_set_meta_data(const void *data,
                                      const uint32_t length) override {
    LOGI() << "on_audio_set_meta_data: " << data;
  }

  virtual void on_audio_stream_started(
      const aps::audio_data_format_t format) override {
    LOGI() << "on_audio_stream_started: " << format;
    init_audio_data_file();
    //append_aac_sequence_header();
  }

  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                    const uint32_t payload_length) override {
    LOGV() << "on_audio_stream_data: " << payload_length;
    append_rtp_data(p->payload, payload_length);
  }

  virtual void on_audio_stream_stopped() override {
    LOGI() << "on_audio_stream_stopped";
  }

private:
  std::string file_id_;

  std::ofstream video_data_file_;
  void init_video_data_file() {
    if (video_data_file_.is_open()) {
      video_data_file_.close();
    }

    std::ostringstream oss;
    oss << file_id_ << "-screen" << ".h264";

    auto mode =
      std::ios_base::binary | std::ios_base::binary | std::ios_base::trunc;
    video_data_file_.open(oss.str(), mode);
  }
  
  void append_avc_sequence_header(const aps::sms_video_codec_packet_t *p) {
    uint32_t sc = htonl(0x01);
    std::ostringstream oss;

    // Parse SPS
    uint8_t *cursor = (uint8_t *)p->decord_record.start;
    for (int i = 0; i < p->decord_record.sps_count; i++) {
      oss.write((char *)&sc, 0x4);
      uint16_t sps_length = *(uint16_t *)cursor;
      sps_length = ntohs(sps_length);
      cursor += sizeof(uint16_t);
      oss.write((char *)cursor, sps_length);
      cursor += sps_length;
    }

    // Parse PPS
    uint8_t pps_count = *cursor++;
    for (int i = 0; i < pps_count; i++) {
      oss.write((char *)&sc, 0x4);
      uint16_t pps_length = *(uint16_t *)cursor;
      pps_length = ntohs(pps_length);
      cursor += sizeof(uint16_t);
      oss.write((char *)cursor, pps_length);
      cursor += pps_length;
    }
    std::string buffer = oss.str();
    video_data_file_.write((char *)buffer.c_str(), buffer.length());
  }

  void append_nalu_data(const aps::sms_video_data_packet_t *p) {
    static uint32_t sc = htonl(0x01);
    video_data_file_.write((char *)&sc, sizeof(uint32_t));
    video_data_file_.write((char *)(p->payload + sizeof(uint32_t)), 
      p->payload_size - sizeof(uint32_t));
  }

  void close_video_data_file() {
    video_data_file_.close();
  }

  aac_eld_file *aac_eld = 0;
  std::ofstream audio_data_file_;
  void init_audio_data_file() {
    aac_eld = create_aac_eld();

    if (audio_data_file_.is_open()) {
      audio_data_file_.close();
    }

    std::ostringstream oss;
    oss << file_id_ << "-audio" << ".dat";

    auto mode =
      std::ios_base::binary | std::ios_base::binary | std::ios_base::trunc;
    audio_data_file_.open(oss.str(), mode);
  }

  void append_aac_sequence_header() {
    // Please refer to: https://wiki.multimedia.cx/index.php?title=MPEG-4_Audio#Audio_Specific_Config
    // 
    // 5 bits: object type
    // if (object type == 31)
    //     6 bits + 32: object type
    // 4 bits: frequency index
    // if (frequency index == 15)
    //     24 bits: frequency
    // 4 bits: channel configuration
    // var bits: AOT Specific Config
    // 
    // 5 bits: 11111    (31)  
    // 6 bits: 000111   (7)   object type = 32 +7 (39, ER AAC ELD)
    // 4 bits: 0100     (4)   frequency index = 4 (44100 Hz)
    // 4 bits: 0010     (2)   channel configuration = 2 (channels: front-left, front-right)
    static uint8_t asc_config[] = { 0xF8, 0xE8, 0x50, 0x00 };
    //audio_data_file_.write((char *)asc_config, sizeof(asc_config));
  }

  void append_rtp_data(const uint8_t *p, int32_t length) {
    std::vector<uint8_t> output(4096);
    int outsize = 0;
    aac_eld_decode_frame(aac_eld, (unsigned char *)p, length, output.data(), &outsize);
    audio_data_file_.write((char *)output.data(), outsize);
  }

  void close_audio_data_file() {
    audio_data_file_.close();
  }
};

class airplay_video_handler : public aps::ap_video_session_handler {
 public:
  long session_;
  airplay_video_handler(long sid) : session_(sid) {};
  ~airplay_video_handler() {};

  virtual void on_video_play(const uint64_t session_id,
                             const std::string &location,
                             const float start_pos) override {
    LOGI() << "on_video_play: " << location << ", session: " << session_id;
    session_ = session_id;
  }

  virtual void on_video_scrub(const uint64_t session_id,
                              const float position) override {
    LOGI() << "on_video_scrub: " << position;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id
             << ", current session: " << session_;
    }
  }

  virtual void on_video_rate(const uint64_t session_id,
                             const float value) override {
    LOGI() << "on_video_rate: " << value;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id
             << ", current session: " << session_;
    }
  }

  virtual void on_video_stop(const uint64_t session_id) override {
    LOGI() << "on_video_stop: ";
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id
             << ", current session: " << session_;
    }
  }

  virtual void on_acquire_playback_info(
      const uint64_t session_id, aps::playback_info_t &playback_info) override {
    LOGV() << "on_acquire_playback_info: ";
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id
             << ", current session: " << session_;
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
    LOGI() << "-------------------on_session_end: " << session_id;
  }

  void on_mirror_session_begin(aps::ap_session_ptr session) {
    uint64_t sid = session->get_session_id();
    LOGI() << "###################on_mirror_session_begin: " << sid;
    aps::ap_mirror_session_handler_ptr mirror_handler =
        std::make_shared<airplay_mirror_handler>();
    session->set_mirror_handler(mirror_handler);
  }

  void on_video_session_begin(aps::ap_session_ptr session) {
    uint64_t sid = session->get_session_id();
    LOGI() << "+++++++++++++++++++on_video_session_begin: " << sid;
    aps::ap_video_session_handler_ptr video_handler =
        std::make_shared<airplay_video_handler>(sid);
    session->set_video_handler(video_handler);
  }
};

airplay_handler::airplay_handler() {}

airplay_handler::~airplay_handler() {}

int main() {
  aps::ap_server_ptr server = std::make_shared<aps::ap_server>();
  aps::ap_handler_ptr handler = std::make_shared<airplay_handler>();
  aps::ap_config_ptr config = aps::ap_config::default_instance();
  config->macAddress("AA:BB:CC:DD:EE:FF");
  config->name("APS[" + config->macAddress() +"]");
  server->set_config(config);
  server->set_handler(handler);
  LOGI() << "AP Server is starting....";
  server->start();
  LOGI() << "AP Server started....";
  getchar();
  server->stop();
  return 0;
}
