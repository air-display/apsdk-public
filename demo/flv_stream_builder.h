#pragma once
#include <vector>

namespace flv {
struct stream_header_s {
  uint32_t signature;
};

struct tag_header_s {};

struct tag_s {};

enum tag_type_e {
  kAudio = 0x08,
  kVideo = 0x09,
  kScript = 0x12,

  kFORCE_UIN8 = (uint8_t)0xff
};
typedef tag_type_e tag_type_t;

class flv_stream_builder {
public:
  flv_stream_builder();
  ~flv_stream_builder();

  void header_block(std::vector<uint8_t> &buf, bool has_audio, bool has_video);

  void meta_block(std::vector<uint8_t> &buf);

  void audio_block(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length);

  void video_block(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length);

protected:
  void init_stream_header(std::vector<uint8_t> &buf, bool has_audio,
                          bool has_video);

  void append_audio_tag(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length);

  void append_video_tag(std::vector<uint8_t> &buf, uint32_t timestamp,
                        const uint8_t *data, uint32_t length);

  void append_tag(std::vector<uint8_t> &buf, tag_type_t type,
                  uint32_t timestamp, uint32_t strem_id, const uint8_t *data,
                  uint32_t length);

  void append_tag_size(std::vector<uint8_t> &buf);

private:
  uint64_t tag_count_;
  bool has_audio_;
  bool has_video_;
};

} // namespace flv
