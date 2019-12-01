// aps-demo.cpp : This file contains the 'main' function. Program execution
// begins and ends there.
//

#include <fstream>
#include <strstream>

#include "audiodec/aac_eld.h"
#include "audiodec/alac.h"

#include "../src/ap_config.h"
#include "../src/ap_server.h"
#include "../src/ap_session.h"
#include "../src/ap_types.h"
#include "../src/utils/logger.h"
#include "../src/utils/packing.h"
#include "../src/utils/utils.h"

#include "flv_stream_builder.hpp"

class airplay_mirror_handler : public aps::ap_mirror_session_handler {
private:
  std::ofstream ofs_;
  flv::flv_stream_builder flv_builder_;
  uint64_t start_time_ms;

public:
  airplay_mirror_handler() : flv_builder_(ofs_) {
    time_t now = time(0);
    std::tm *local_now = localtime(&now);
    std::ostringstream oss;
    oss << local_now->tm_year + 1900 << "-" << local_now->tm_mon + 1 << "-" << local_now->tm_mday << "-"
        << local_now->tm_hour << "-" << local_now->tm_min << "-" << local_now->tm_sec;
    file_id_ = oss.str();

    ofs_.open("mirror-data.flv", std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
  };
  ~airplay_mirror_handler(){};

  virtual void on_mirror_stream_started() override {
    LOGI() << "on_mirror_stream_started";
    init_video_data_file();
    flv_builder_.init_stream_header(true, true);

    auto meta = flv::amf::amf_array::create()
                    ->with_item("audiocodecid", (double)10)
                    ->with_item("audiosamplerate", (double)44100)
                    ->with_item("audiosamplesize", (double)16)
                    ->with_item("stereo", true)
                    ->with_item("videocodecid", (double)7)
        //->with_item("framerate", (double)30)
        //->with_item("videodatarate", (double)520)
        //->with_item("width", (double)1920)
        //->with_item("height", (double)1080)
        //->with_item("filesize", (double)0)
        ;

    flv_builder_.append_meta_tag(meta);
  }

  virtual void on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p) override {
    LOGI() << "on_mirror_stream_codec: ";
    start_time_ms = normalize_ntp_to_ms(p->timestamp);
    flv_builder_.append_video_tag_with_avc_decoder_config(0, p->payload, p->payload_size);
    append_avc_sequence_header(p);
  }

  virtual void on_mirror_stream_data(const aps::sms_video_data_packet_t *p) override {
    uint64_t timestamp_ms = normalize_ntp_to_ms(p->timestamp);
    timestamp_ms -= start_time_ms;
    flv_builder_.append_video_tag_with_avc_nalu_data(timestamp_ms, p->payload, p->payload_size);
    append_nalu_data(p);

    LOGI() << "on_mirror_stream_data payload_size: " << p->payload_size << ", timestamp: " << timestamp_ms;
  }

  virtual void on_mirror_stream_stopped() override {
    LOGI() << "on_mirror_stream_stopped";
    close_video_data_file();
  }

  virtual void on_audio_set_volume(const float ratio, const float volume) override {
    LOGI() << "on_audio_set_volume: " << ratio << ", value: " << volume;
  }

  virtual void on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current,
                                     const uint64_t end) override {
    LOGI() << "on_audio_set_progress: " << ratio << ", start: " << start << ", current: " << current
           << ", end: " << end;
  }

  virtual void on_audio_set_cover(const std::string format, const void *data, const uint32_t length) override {
    LOGI() << "on_audio_set_cover: " << format;
  }

  virtual void on_audio_set_meta_data(const void *data, const uint32_t length) override {
    LOGI() << "on_audio_set_meta_data: " << data;
  }

  virtual void on_audio_stream_started(const aps::audio_data_format_t format) override {
    LOGI() << "on_audio_stream_started: " << format;
    this->format = format;
    init_audio_data_file();
    uint8_t asc[] = {0xF8, 0xE8, 0x50, 0x00};
    flv_builder_.append_audio_tag_with_aac_specific_config(0,
                                                           flv::audio_data_sound_rate_t::R44KHZ,
                                                           flv::audio_data_sound_size_t::S16BIT,
                                                           flv::audio_data_sound_type_t::STEREO,
                                                           asc,
                                                           4);
  }

  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length) override {
    LOGV() << "on_audio_stream_data: " << payload_length << ", timestamp: " << p->timestamp;
    append_rtp_data(p->payload, payload_length);
    flv_builder_.append_audio_tag_with_aac_frame_data(0,
                                                      flv::audio_data_sound_rate_t::R44KHZ,
                                                      flv::audio_data_sound_size_t::S16BIT,
                                                      flv::audio_data_sound_type_t::STEREO,
                                                      p->payload,
                                                      payload_length);
  }

  virtual void on_audio_stream_stopped() override { LOGI() << "on_audio_stream_stopped"; }

  virtual void on_mirror_stream_heartbeat() override { LOGD() << "on_mirror_stream_heartbeat"; }

private:
  std::string file_id_;

  std::ofstream video_data_file_;
  void init_video_data_file() {
    if (video_data_file_.is_open()) {
      video_data_file_.close();
    }

    std::ostringstream oss;
    oss << file_id_ << "-screen"
        << ".h264";

    auto mode = std::ios_base::binary | std::ios_base::binary | std::ios_base::trunc;
    video_data_file_.open(oss.str(), mode);
  }

  void append_avc_sequence_header(const aps::sms_video_codec_packet_t *p) {
    // Convert from H264 AVCC format to H264 Annex-B format
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
    // Convert from H264 AVCC format to H264 Annex-B format
    static uint32_t sc = htonl(0x01);
    video_data_file_.write((char *)&sc, sizeof(uint32_t));
    video_data_file_.write((char *)(p->payload + sizeof(uint32_t)), p->payload_size - sizeof(uint32_t));
  }

  void close_video_data_file() { video_data_file_.close(); }

  aps::audio_data_format_t format;
  aps::alac_context alac_ctx = 0;
  aps::aaceld_context aaceld_ctx = 0;
  std::ofstream audio_data_file_;
  void init_audio_data_file() {
    std::ostringstream oss;

    if (format == aps::audio_format_alac) {
      alac_ctx = aps::create_alac_decoder();
      oss << file_id_ << "-audio"
          << ".alac.pcm";
    } else if (format == aps::audio_format_aac_eld) {
      aaceld_ctx = aps::create_aaceld_decoder();
      oss << file_id_ << "-audio"
          << ".aac-eld.pcm";
    }

    if (audio_data_file_.is_open()) {
      audio_data_file_.close();
    }

    auto mode = std::ios_base::binary | std::ios_base::binary | std::ios_base::trunc;
    audio_data_file_.open(oss.str(), mode);
  }

  void append_rtp_data(const uint8_t *p, int32_t length) {
    std::vector<uint8_t> output(4096);
    int outsize = 0;

    if (format == aps::audio_format_alac) {
      aps::alac_decode_frame(alac_ctx, (unsigned char *)p, output.data(), &outsize);
    } else if (format == aps::audio_format_aac_eld) {
      aps::aaceld_decode_frame(aaceld_ctx, (unsigned char *)p, length, output.data(), &outsize);
    }

    audio_data_file_.write((char *)output.data(), outsize);
  }

  void close_audio_data_file() {
    audio_data_file_.close();
    if (alac_ctx) {
      aps::destory_alac_decoder(alac_ctx);
    }

    if (aaceld_ctx) {
      aps::destroy_aaceld_decoder(aaceld_ctx);
    }
  }
};

class airplay_video_handler : public aps::ap_video_session_handler {
public:
  uint64_t session_;
  airplay_video_handler(uint64_t sid) : session_(sid){};
  ~airplay_video_handler(){};

  virtual void on_video_play(const uint64_t session_id, const std::string &location, const float start_pos) override {
    LOGI() << "on_video_play: " << location << ", session: " << session_id;
    session_ = session_id;
  }

  virtual void on_video_scrub(const uint64_t session_id, const float position) override {
    LOGI() << "on_video_scrub: " << position;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_;
    }
  }

  virtual void on_video_rate(const uint64_t session_id, const float value) override {
    LOGI() << "on_video_rate: " << value;
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_;
    }
  }

  virtual void on_video_stop(const uint64_t session_id) override {
    LOGI() << "on_video_stop: ";
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_;
    }
  }

  virtual void on_acquire_playback_info(const uint64_t session_id, aps::playback_info_t &playback_info) override {
    LOGV() << "on_acquire_playback_info: ";
    if (session_ != session_id) {
      LOGE() << "Invalid session id: " << session_id << ", current session: " << session_;
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
    aps::ap_mirror_session_handler_ptr mirror_handler = std::make_shared<airplay_mirror_handler>();
    session->set_mirror_handler(mirror_handler);
  }

  void on_video_session_begin(aps::ap_session_ptr session) {
    uint64_t sid = session->get_session_id();
    LOGI() << "+++++++++++++++++++on_video_session_begin: " << sid;
    aps::ap_video_session_handler_ptr video_handler = std::make_shared<airplay_video_handler>(sid);
    session->set_video_handler(video_handler);
  }
};

airplay_handler::airplay_handler() {}

airplay_handler::~airplay_handler() {}

int main() {
  logger::init_logger(false, log_level::LL_INFO);
  aps::ap_server_ptr server = std::make_shared<aps::ap_server>();
  aps::ap_handler_ptr handler = std::make_shared<airplay_handler>();
  aps::ap_config_ptr config = aps::ap_config::default_instance();
  // config->features(0x527FF18D);  //0x527FFFF7
  config->macAddress("AA:BB:CC:DD:EE:FF");
  config->name("APS[" + config->macAddress() + "]");
  server->set_config(config);
  server->set_handler(handler);
  LOGI() << "AP Server is starting....";
  server->start();
  LOGI() << "AP Server started....";
  getchar();
  server->stop();
  return 0;
}
