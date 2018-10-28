#include <sstream>
#include "ap_config.h"

namespace aps {
    ap_config_audio_format::ap_config_audio_format()
    {
        type_ = 0;
        audioInputFormats_ = 0;
        audioOutputFormats_ = 0;
    }

    ap_config_audio_format::~ap_config_audio_format()
    {
        type_ = 0;
        audioInputFormats_ = 0;
        audioOutputFormats_ = 0;
    }

    ap_config_audio_latency::ap_config_audio_latency()
    {
        type_ = 0;
        inputLatencyMicros_ = 0;
        outputLatencyMicros_ = 0;
        audioType_.clear();
    }

    ap_config_audio_latency::~ap_config_audio_latency()
    {
        type_ = 0;
        inputLatencyMicros_ = 0;
        outputLatencyMicros_ = 0;
        audioType_.clear();
    }

    ap_config_display::ap_config_display()
    {
        width_ = 0;
        height_ = 0;
        refreshRate_ = 0;
        uuid_.clear();
    }

    ap_config_display::~ap_config_display()
    {
        width_ = 0;
        height_ = 0;
        refreshRate_ = 0;
        uuid_.clear();
    }

    ap_config& ap_config::default_instance()
    {
        static ap_config s_instance;
        static bool s_initialized = false;
        if (!s_initialized)
        {
            s_instance.name_ = "Tencent WeCast Display";
            s_instance.deviceID_ = "00:00:00:00:00:00";
            s_instance.model_ = "AppleTV3,2";
            s_instance.sourceVersion_ = "220.68";
            s_instance.pi_ = "b08f5a79-db29-4384-b456-a4784d9e6055";
            s_instance.pk_ = "99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63";
            s_instance.macAddress_ = s_instance.deviceID_;
            s_instance.vv_ = 2;
            s_instance.features_ = 0x5A7FDFD1;
            s_instance.statusFlag_ = 68;

            s_instance.display_.width(1920);
            s_instance.display_.height(1080);
            s_instance.display_.refreshRate(1.0f / 24);
            s_instance.display_.uuid("e5f7a68d-7b0f-4305-984b-974f677a150b");

            s_instance.audio_latency_.type(96);
            s_instance.audio_latency_.audioType("default");
            s_instance.audio_latency_.inputLatencyMicros(3);
            s_instance.audio_latency_.outputLatencyMicros(79);

            s_instance.audio_format_.type(96);
            s_instance.audio_format_.audioInputFormats(0x01000000);
            s_instance.audio_format_.audioOutputFormats(0x01000000);

            s_initialized = true;
        }
        return s_instance;
    }

    ap_config::ap_config()
    {
    }

    ap_config::~ap_config()
    {
    }

    const std::string ap_config::features_hex_string() const
    {
        std::ostringstream oss;
        oss << "0x" << std::hex << features_;
        return oss.str();
    }
}
