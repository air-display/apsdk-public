#include "flv_stream_builder.h"

static const uint8_t FLV_HEADER_SIZE = 9;
static const uint8_t FLV_TAG_HEADER_SIZE = 11;
static const uint8_t VIDEO_HEADER_SIZE = 5;
static const uint8_t VIDEO_SPECIFIC_CONFIG_EXTENDED_SIZE = 11;
static const uint8_t AUDIO_HEADER_SIZE = 2;
static const uint8_t AUDIO_SPECIFIC_CONFIG_SIZE = 2;
static const char *ON_META_DATA = "onMetaData";
static const uint8_t ON_META_DATA_LENGTH = 0x0a;

enum audio_data_sound_format {
  LPCM_PE = 0,
  ADPCM = 1,
  MP3 = 2,
  LPCM_LE = 3,
  NELLYMOSER_16K_MONO = 4,
  NELLYMOSER_8K_MONO = 5,
  NELLYMOSER = 6,
  G711_A = 7,
  G711_MU = 8,
  RESERVERD = 9,
  AAC = 10,
  SPEEX = 11,
  MP3_8K = 14,
  DEVICE_SPECIFIC = 15,
};

enum audio_data_sound_rate {
  R5K5HZ = 0,
  R11KHZ = 1,
  R22KHZ = 2,
  R44KHZ = 3,
};

enum audio_data_sound_size {
  S8BIT = 0,
  S16BIT = 1,
};

enum audio_data_sound_type {
  MONO = 0,
  STEREO = 1,
};

enum aac_audio_data_packet_type {
  AacSequenceHeader = 0,
  AacRaw = 1,
};

enum video_data_frame_type {
  KEY_FRAME = 1,
  INTER_FRAME = 2,
  DISPOSABLE_INTER_FRAME = 3,
  GENERATED_KEY_FRAME = 4,
  VIDEO_INFO_COMMAND_FRAME = 5,
};

enum video_data_codec_id {
  JPEG = 1,
  H263 = 2,
  SCREEN_VIDEO = 3,
  ON2VP6 = 4,
  ON2VP6_WITH_ALPHA = 5,
  SCREEN_VIDEO_V2 = 6,
  AVC = 7,
};

enum avc_video_packet_type {
  AvcSequenceHeader = 0,
  AvcNALU = 1,
  AvcSequenceHeaderEOF = 2,
};

using namespace flv::amf;

flv::flv_stream_builder::flv_stream_builder() {}

flv::flv_stream_builder::~flv_stream_builder() {}

void flv::flv_stream_builder::init_stream_header(std::vector<uint8_t> &buf,
                                                 bool has_audio,
                                                 bool has_video) {
  buf.clear();
  buf.resize(9 + 4, 0);

  uint8_t flags = 0;
  has_audio_ = has_audio;
  if (has_audio_) {
    flags |= 0x04;
  }
  has_video_ = has_video;
  if (has_video_) {
    flags |= 0x01;
  }

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
  buf[8] = FLV_HEADER_SIZE;

  buf[9] = 0;
  buf[10] = 0;
  buf[11] = 0;
  buf[12] = 0;
}

void flv::flv_stream_builder::append_meta_tag(std::vector<uint8_t> &buf,
                                              amf::amf_value_ref meta) {
  std::vector<uint8_t> meta_data;
  amf_string::create(ON_META_DATA)->serialize(meta_data);
  meta->serialize(meta_data);
  append_tag(buf, Script, 0, 0, meta_data.data(), meta_data.size());
}

void flv::flv_stream_builder::append_video_tag(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_tag(buf, Video, timestamp, 0, data, length);
}

void flv::flv_stream_builder::append_audio_tag(std::vector<uint8_t> &buf,
                                               uint32_t timestamp,
                                               const uint8_t *data,
                                               uint32_t length) {
  append_tag(buf, Audio, timestamp, 0, data, length);
}

void flv::flv_stream_builder::append_tag(std::vector<uint8_t> &buf,
                                         tag_type_t type, uint32_t timestamp,
                                         uint32_t strem_id, const uint8_t *data,
                                         uint32_t length) {
  int32_t original_size = buf.size();
  buf.reserve(buf.size() + FLV_TAG_HEADER_SIZE + length);

  // Header.Type
  buf.emplace_back(type);

  // Header.DataSize
  buf.emplace_back((length & 0x00ff0000) >> 16);
  buf.emplace_back((length & 0x0000ff00) >> 8);
  buf.emplace_back((length & 0x000000ff));

  // Header.Timestamp
  buf.emplace_back((timestamp & 0x00ff0000) >> 16);
  buf.emplace_back((timestamp & 0x0000ff00) >> 8);
  buf.emplace_back((timestamp & 0x000000ff));

  // Header.TimestampExtended
  buf.emplace_back((timestamp & 0xff000000) >> 24);

  // Header.StreamID (actually this is always 0 according to the specification)
  buf.emplace_back((strem_id & 0x00ff0000) >> 16);
  buf.emplace_back((strem_id & 0x0000ff00) >> 8);
  buf.emplace_back((strem_id & 0x000000ff));

  // Data
  std::copy(data, data + length, std::back_inserter(buf));

  uint32_t tag_size = buf.size() - original_size;
  append_tag_size(buf, tag_size);
}

void flv::flv_stream_builder::append_tag_size(std::vector<uint8_t> &buf,
                                              uint32_t length) {
  buf.reserve(buf.size() + 4);
  buf.emplace_back((length & 0xff000000) >> 24);
  buf.emplace_back((length & 0x00ff0000) >> 16);
  buf.emplace_back((length & 0x0000ff00) >> 8);
  buf.emplace_back((length & 0x000000ff));
}
