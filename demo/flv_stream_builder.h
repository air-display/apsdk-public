#pragma once
#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include <atomic>

namespace amf {
enum amf_value_type_e {
  NumberType = 0,  // 8 bytes
  BooleanType = 1, // 1 bytes
  StringType = 2,
  ObjectType = 3,
  MovieClipType = 4,
  NullType = 5,
  UndefinedType = 6,
  ReferenceType = 7,
  ECMAArrayType = 8,
  StrictArrayType = 10,
  DateType = 11,
  LongStringType = 12,
};
typedef amf_value_type_e amf_value_type_t;

class amf_root {
public:
  virtual ~amf_root(){};

  virtual amf_value_type_t value_type() const = 0;

  virtual void serialize(std::vector<uint8_t> &buf) = 0;

  virtual bool deserialize(const std::vector<uint8_t> &data) = 0;
};
typedef std::shared_ptr<amf_root> amf_root_ref;

class amf_value : amf_root {
protected:
  amf_value_type_t type;

public:
  amf_value(amf_value_type_t t) : type(t){};
  ~amf_value() {}

  virtual amf_value_type_t value_type() const override { return type; }

  virtual void serialize(std::vector<uint8_t> &buf) override {
    throw std::logic_error("The method or operation is not implemented.");
  }

  virtual bool deserialize(const std::vector<uint8_t> &data) override {
    throw std::logic_error("The method or operation is not implemented.");
  }
};
typedef std::shared_ptr<amf_value> amf_value_ref;

class amf_number;
typedef std::shared_ptr<amf_number> amf_number_ref;
class amf_number : public amf_value {
public:
  static amf_number_ref create(double value) {
    return std::make_shared<amf_number>(value);
  }
  amf_number(double value) : amf_value(NumberType), v(value){};
  virtual void serialize(std::vector<uint8_t> &buf) override {
    buf.reserve(buf.size() + 1 + 8);
    buf.emplace_back(type);
    uint8_t *p = (uint8_t *)&v;
    int i = 8;
    do {
      buf.emplace_back(p[--i]);
    } while (i);
  }

private:
  double v;
};

class amf_boolean;
typedef std::shared_ptr<amf_boolean> amf_boolean_ref;
class amf_boolean : public amf_value {
public:
  static amf_boolean_ref create(bool value) {
    return std::make_shared<amf_boolean>(value);
  }
  amf_boolean(bool value) : amf_value(BooleanType), v(value){};
  virtual void serialize(std::vector<uint8_t> &buf) override {
    buf.reserve(buf.size() + 1 + 1);
    buf.emplace_back(type);
    buf.emplace_back(v);
  }

private:
  bool v;
};

class amf_string;
typedef std::shared_ptr<amf_string> amf_string_ref;
class amf_string : public amf_value {
public:
  static amf_string_ref create(const char *value) {
    return std::make_shared<amf_string>(value);
  }
  amf_string(const char *value) : amf_value(StringType), v(value){};
  virtual void serialize(std::vector<uint8_t> &buf) override {
    uint16_t length = v.length();
    buf.reserve(buf.size() + 1 + 2 + length);
    buf.emplace_back(type);
    buf.emplace_back(length >> 8);
    buf.emplace_back(length & 0x00ff);
    std::copy(v.begin(), v.begin() + length, std::back_inserter(buf));
  }

private:
  std::string v;
};

class amf_object;
typedef std::shared_ptr<amf_object> amf_object_ref;
class amf_object : public amf_value,
                   public std::enable_shared_from_this<amf_object> {
public:
  static amf_object_ref create() { return std::make_shared<amf_object>(); }

  amf_object() : amf_value(ObjectType){};

  amf_object_ref with_property(const char *key, double v) {
    auto pv = amf_number::create(v);
    return this->with_property(key, pv);
  }

  amf_object_ref with_property(const char *key, bool v) {
    auto pv = amf_boolean::create(v);
    return this->with_property(key, pv);
  }

  amf_object_ref with_property(const char *key, const char *v) {
    auto pv = amf_string::create(v);
    return this->with_property(key, pv);
  }

  amf_object_ref with_property(const char *key, amf_value_ref v) {
    this->v[key] = v;
    return shared_from_this();
  }

  virtual void serialize(std::vector<uint8_t> &buf) override {
    buf.reserve(buf.size() + 1);
    buf.emplace_back(type);

    uint16_t key_length = 0;
    for (auto &kv : v) {
      key_length = kv.first.length();
      buf.emplace_back(key_length >> 8);
      buf.emplace_back(key_length & 0x0ff);
      std::copy(kv.first.begin(), kv.first.begin() + key_length,
                std::back_inserter(buf));

      kv.second->serialize(buf);
    }

    buf.emplace_back(0);
    buf.emplace_back(0);
    buf.emplace_back(9);
  }

protected:
  std::map<std::string, amf_value_ref> v;
};

class amf_array;
typedef std::shared_ptr<amf_array> amf_array_ref;
class amf_array : public amf_value,
                  public std::enable_shared_from_this<amf_array> {
public:
  static amf_array_ref create() { return std::make_shared<amf_array>(); }
  amf_array() : amf_value(ECMAArrayType){};

  amf_array_ref with_item(const char *key, double v) {
    auto pv = amf_number::create(v);
    return this->with_item(key, pv);
  }

  amf_array_ref with_item(const char *key, bool v) {
    auto pv = amf_boolean::create(v);
    return this->with_item(key, pv);
  }

  amf_array_ref with_item(const char *key, const char *v) {
    auto pv = amf_string::create(v);
    return this->with_item(key, pv);
  }

  amf_array_ref with_item(const char *key, amf_value_ref v) {
    this->v[key] = v;
    return shared_from_this();
  }

  virtual void serialize(std::vector<uint8_t> &buf) override {
    buf.reserve(buf.size() + 1);
    buf.emplace_back(type);
    uint32_t count = v.size();
    buf.emplace_back((count & 0xff000000) >> 24);
    buf.emplace_back((count & 0x00ff0000) >> 16);
    buf.emplace_back((count & 0x0000ff00) >> 8);
    buf.emplace_back((count & 0x000000ff));

    uint16_t key_length = 0;
    for (auto &kv : v) {
      key_length = kv.first.length();
      buf.emplace_back(key_length >> 8);
      buf.emplace_back(key_length & 0x0ff);
      std::copy(kv.first.begin(), kv.first.begin() + key_length,
                std::back_inserter(buf));

      kv.second->serialize(buf);
    }

    buf.emplace_back(0);
    buf.emplace_back(0);
    buf.emplace_back(9);
  }

protected:
  std::map<std::string, amf_value_ref> v;
};
} // namespace amf

using namespace amf;

namespace flv {
static const uint8_t FLV_HEADER_SIZE = 9;
static const uint8_t FLV_TAG_HEADER_SIZE = 11;
static const uint8_t VIDEO_HEADER_SIZE = 5;
static const uint8_t VIDEO_SPECIFIC_CONFIG_EXTENDED_SIZE = 11;
static const uint8_t AUDIO_HEADER_SIZE = 2;
static const uint8_t AUDIO_SPECIFIC_CONFIG_SIZE = 2;
static const char *ON_META_DATA = "onMetaData";
static const uint8_t ON_META_DATA_LENGTH = 0x0a;

enum tag_type_e {
  Unknown = 0,
  Audio = 0x08,
  Video = 0x09,
  Script = 0x12,

  FORCE_UINT8 = (uint8_t)0xff
};
typedef tag_type_e tag_type_t;

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

enum audio_data_sound_rate_e {
  R5K5HZ = 0,
  R11KHZ = 1,
  R22KHZ = 2,
  R44KHZ = 3,
};
typedef audio_data_sound_rate_e audio_data_sound_rate_t;

enum audio_data_sound_size_e {
  S8BIT = 0,
  S16BIT = 1,
};
typedef audio_data_sound_size_e audio_data_sound_size_t;

enum audio_data_sound_type_e {
  MONO = 0,
  STEREO = 1,
};
typedef audio_data_sound_type_e audio_data_sound_type_t;

class flv_stream_builder {
public:
  flv_stream_builder() {}
  ~flv_stream_builder() {}

  void init_stream_header(std::vector<uint8_t> &buf, bool has_audio,
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

  void append_meta_tag(std::vector<uint8_t> &buf, amf::amf_value_ref meta) {
    std::vector<uint8_t> meta_data;
    amf_string::create(ON_META_DATA)->serialize(meta_data);
    meta->serialize(meta_data);
    append_tag(buf, Script, 0, 0, meta_data.data(), meta_data.size());
  }

  void append_video_tag(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length) {
    append_tag(buf, Video, timestamp, 0, data, length);
  }

  void append_avc_decoder_config_video_tag(std::vector<uint8_t> &buf,
                                           uint32_t timestamp,
                                           const uint8_t *data,
                                           uint32_t length) {
    std::vector<uint8_t> avc_packet;
    avc_packet.reserve(512);
    avc_packet.emplace_back(INTER_FRAME << 4 | AVC);
    avc_packet.emplace_back(AvcSequenceHeader);
    avc_packet.emplace_back(0);
    avc_packet.emplace_back(0);
    avc_packet.emplace_back(0);
    std::copy(data, data + length, std::back_inserter(avc_packet));
    append_video_tag(buf, timestamp, avc_packet.data(), avc_packet.size());
  }

  void append_avc_nalu_data_video_tag(std::vector<uint8_t> &buf,
                                      uint32_t timestamp, const uint8_t *data,
                                      uint32_t length) {
    std::vector<uint8_t> avc_packet;
    avc_packet.reserve(512);
    avc_packet.emplace_back(INTER_FRAME << 4 | AVC);
    avc_packet.emplace_back(AvcNALU);
    uint32_t composition_time = 0;
    avc_packet.emplace_back((composition_time & 0x00ff0000) >> 16);
    avc_packet.emplace_back((composition_time & 0x0000ff00) >> 8);
    avc_packet.emplace_back((composition_time & 0x000000ff));
    std::copy(data, data + length, std::back_inserter(avc_packet));
    append_video_tag(buf, timestamp, avc_packet.data(), avc_packet.size());
  }

  void append_aac_specific_config_audio_tag(
      std::vector<uint8_t> &buf, uint32_t timestamp,
      audio_data_sound_rate_t rate, audio_data_sound_size_t size,
      audio_data_sound_type_t type, const uint8_t *data, uint32_t length) {
    std::vector<uint8_t> aac_packet;
    aac_packet.reserve(32);
    uint8_t fb = AAC << 4;
    fb |= ((rate << 2) & 0x06);
    fb |= ((size << 1) & 0x02);
    fb |= ((type)&0x01);
    aac_packet.emplace_back(fb);
    aac_packet.emplace_back(AacSequenceHeader);
    std::copy(data, data + length, std::back_inserter(aac_packet));
    append_audio_tag(buf, timestamp, aac_packet.data(), aac_packet.size());
  }

  void append_aac_frame_data_audio_tag(std::vector<uint8_t> &buf,
                                       uint32_t timestamp,
                                       audio_data_sound_rate_t rate,
                                       audio_data_sound_size_t size,
                                       audio_data_sound_type_t type,
                                       const uint8_t *data, uint32_t length) {
    std::vector<uint8_t> aac_packet;
    aac_packet.reserve(32);
    uint8_t fb = AAC << 4;
    fb |= ((rate << 2) & 0x06);
    fb |= ((size << 1) & 0x02);
    fb |= ((type)&0x01);
    aac_packet.emplace_back(fb);
    aac_packet.emplace_back(AacRaw);
    std::copy(data, data + length, std::back_inserter(aac_packet));
    append_audio_tag(buf, timestamp, aac_packet.data(), aac_packet.size());
  }

  void append_audio_tag(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length) {
    append_tag(buf, Audio, timestamp, 0, data, length);
  }

protected:
  void append_tag(std::vector<uint8_t> &buf, tag_type_t type,
                  uint32_t timestamp, uint32_t strem_id, const uint8_t *data,
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

    // Header.StreamID (actually this is always 0 according to the
    // specification)
    buf.emplace_back((strem_id & 0x00ff0000) >> 16);
    buf.emplace_back((strem_id & 0x0000ff00) >> 8);
    buf.emplace_back((strem_id & 0x000000ff));

    // Data
    std::copy(data, data + length, std::back_inserter(buf));

    uint32_t tag_size = buf.size() - original_size;
    append_tag_size(buf, tag_size);

    tag_count_++;
  }

  void append_tag_size(std::vector<uint8_t> &buf, uint32_t length) {
    buf.reserve(buf.size() + 4);
    buf.emplace_back((length & 0xff000000) >> 24);
    buf.emplace_back((length & 0x00ff0000) >> 16);
    buf.emplace_back((length & 0x0000ff00) >> 8);
    buf.emplace_back((length & 0x000000ff));
  }

private:
  std::atomic<uint64_t> tag_count_;
  bool has_audio_;
  bool has_video_;
};

namespace test {
static void generate_flv_file() {
  auto meta = amf::amf_array::create()
                  //->with_item("duration", (double)0)
                  ->with_item("width", (double)1920)
                  ->with_item("height", (double)1080)
                  //->with_item("videodatarate", (double)520)
                  ->with_item("framerate", (double)25)
                  ->with_item("videocodecid", (double)7)
                  ->with_item("audiosamplerate", (double)44100)
                  ->with_item("audiosamplesize", (double)16)
                  ->with_item("stereo", true)
                  ->with_item("audiocodecid", (double)10)
      //->with_item("filesize", (double)0)
      ;

  std::vector<uint8_t> buf;
  flv_stream_builder builder;
  builder.init_stream_header(buf, true, true);
  builder.append_meta_tag(buf, meta);
  builder.append_video_tag(buf, 0, 0, 0);
  builder.append_audio_tag(buf, 0, 0, 0);

  std::ofstream ofs;
  ofs.open("test_flv_data.flv",
           std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
  ofs.write((char *)buf.data(), buf.size());
  ofs.close();
}
} // namespace test
} // namespace flv
