#include "flv_stream_builder.h"

flv::flv_stream_builder::flv_stream_builder() {}

flv::flv_stream_builder::~flv_stream_builder() {}

void flv::flv_stream_builder::header_block(std::vector<uint8_t> &buf,
                                           bool has_audio, bool has_video) {
  init_stream_header(buf, has_audio, has_video);
  buf.push_back(0);
  buf.push_back(0);
  buf.push_back(0);
  buf.push_back(0);
}

void flv::flv_stream_builder::audio_block(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_audio_tag(buf, timestamp, data, length);
  append_tag_size(buf);
}

void flv::flv_stream_builder::video_block(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_video_tag(buf, timestamp, data, length);
  append_tag_size(buf);
}

void flv::flv_stream_builder::init_stream_header(std::vector<uint8_t> &buf,
                                                 bool has_audio,
                                                 bool has_video) {
  uint8_t flags = 0;
  has_audio_ = has_audio;
  if (has_audio_) {
    flags |= 0x40;
  }
  has_video_ = has_video;
  if (has_video_) {
    flags |= 0x10;
  }
  buf.clear();
  buf.resize(9);

  // Signature
  buf[0] = 'F';
  buf[1] = 'L';
  buf[2] = 'V';

  // Version
  buf[3] = 0x01;

  // Flags
  buf[4] = flags;

  // Header size
  buf[5] = 0;
  buf[6] = 0;
  buf[7] = 0;
  buf[8] = 9;
}

void flv::flv_stream_builder::append_audio_tag(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_tag(buf, kAudio, timestamp, 0, data, length);
}

void flv::flv_stream_builder::append_video_tag(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_tag(buf, kVideo, timestamp, 0, data, length);
}

void flv::flv_stream_builder::append_tag(std::vector<uint8_t> &buf,
                                         tag_type_t type, uint32_t timestamp,
                                         uint32_t strem_id, const uint8_t *data,
                                         uint32_t length) {
  buf.clear();
  buf.resize(length + 1 + 3 + 3 + 1 + 3);

  // Header.Type
  buf[0] = type;

  // Header.DataSize

  // Header.Timestamp

  // Header.StreamID

  // Data
}

void flv::flv_stream_builder::append_tag_size(std::vector<uint8_t> &buf) {
  uint32_t size = buf.size();
  buf.push_back(size >> 24);
  buf.push_back((size & 0x00ff0000) >> 16);
  buf.push_back((size & 0x0000ff00) >> 8);
  buf.push_back(size & 0x000000ff);
}
