#include <algorithm>
#include <sstream>

#include <ap_config.h>
#include <utils/utils.h>

namespace aps {
ap_config_audio_format::ap_config_audio_format() {
  type_ = 0;
  audioInputFormats_ = 0;
  audioOutputFormats_ = 0;
}

ap_config_audio_format::~ap_config_audio_format() {
  type_ = 0;
  audioInputFormats_ = 0;
  audioOutputFormats_ = 0;
}

ap_config_audio_latency::ap_config_audio_latency() {
  type_ = 0;
  inputLatencyMicros_ = 0;
  outputLatencyMicros_ = 0;
  audioType_.clear();
}

ap_config_audio_latency::~ap_config_audio_latency() {
  type_ = 0;
  inputLatencyMicros_ = 0;
  outputLatencyMicros_ = 0;
  audioType_.clear();
}

ap_config_display::ap_config_display() {
  width_ = 0;
  height_ = 0;
  refreshRate_ = 0;
  uuid_.clear();
}

ap_config_display::~ap_config_display() {
  width_ = 0;
  height_ = 0;
  refreshRate_ = 0;
  uuid_.clear();
}

ap_config_ptr ap_config::default_instance() {
  static ap_config_ptr s_instance = 0;
  if (!s_instance) {
    std::string mac_address = generate_mac_address();

    s_instance = std::make_shared<ap_config>();
    s_instance->name_ = "APS Display";
    s_instance->publishService_ = true;
    s_instance->model_ = "AppleTV3,2";
    s_instance->serverVersion_ = "220.68";
    s_instance->pi_ = "b08f5a79-db29-4384-b456-a4784d9e6055";
    s_instance->pk_ =
        "99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63";
    s_instance->flags_ = "0x04";
    s_instance->macAddress_ = mac_address;
    s_instance->vv_ = 2;
    s_instance->features_ =
        0x527FFFF7; // 0x0E5A7FFFF7 with pv // 0x0E527FFFF7 w/o pv;
    s_instance->statusFlag_ = 68;
    s_instance->audioCodecs_ = "0,1,2,3";
    s_instance->encryptionTypes_ = "0,3,5";
    s_instance->metadataTypes_ = "0,1,2";
    s_instance->needPassword_ = "false";
    s_instance->transmissionProtocol_ = "UDP";
    s_instance->display_.width(1920);
    s_instance->display_.height(1080);
    s_instance->display_.refreshRate(60);
    s_instance->display_.uuid("e5f7a68d-7b0f-4305-984b-974f677a150b");
    s_instance->audio_latency_.type(96);
    s_instance->audio_latency_.audioType("default");
    s_instance->audio_latency_.inputLatencyMicros(3);
    s_instance->audio_latency_.outputLatencyMicros(79);
    s_instance->audio_format_.type(96);
    s_instance->audio_format_.audioInputFormats(0x01000000);
    s_instance->audio_format_.audioOutputFormats(0x01000000);
  }

  return s_instance;
}

ap_config::ap_config() {}

ap_config::~ap_config() {}

const std::string ap_config::features_hex_string() const {
  std::ostringstream oss;
  oss << "0x" << std::hex << std::uppercase
      << (uint32_t)(features_ & 0xFFFFFFFF);
  if (features_ & 0xFFFFFFFF00000000)
    oss << ",0x" << std::hex << std::uppercase << (uint32_t)(features_ >> 32);
  return oss.str();
}

const std::string ap_config::deviceID() const {
  return string_replace(macAddress_.c_str(), ":", "");
}

} // namespace aps
