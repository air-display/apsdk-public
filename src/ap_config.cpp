#include "ap_config.h"

namespace aps {
    ap_config_audio_format::ap_config_audio_format()
    {
        type_ = 96;
        audioInputFormats_ = 0x01000000;
        audioOutputFormats_ = 0x01000000;
    }

    ap_config_audio_format::~ap_config_audio_format()
    {

    }

    ap_config_audio_latency::ap_config_audio_latency()
    {
        type_ = 96;
        audioType_ = "default";
        inputLatencyMicros_ = 3;
        outputLatencyMicros_ = 79;
    }

    ap_config_audio_latency::~ap_config_audio_latency()
    {

    }

    ap_config_display::ap_config_display()
    {
        width_ = 1920;
        height_ =1080;
        refreshRate_ = 1.0 / 24;
        uuid_ = "e5f7a68d-7b0f-4305-984b-974f677a150b";
    }

    ap_config_display::~ap_config_display()
    {
    }

    ap_config& ap_config::get()
    {
        static ap_config s_instance;
        return s_instance;
    }

    void ap_config::init()
    {
    }

    ap_config::ap_config()
    {
        name_ = "WeCast Display";
        deviceID_ = "C02WF0AXJ1GQ";
        model_ = "AppleTV3,2";
        sourceVersion_ = "220.68";
        pi_ = "b08f5a79-db29-4384-b456-a4784d9e6055";
        pk_ = "99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63";
        macAddress_ = deviceID_;
        vv_ = 2;
        features_ = 0x5A7FDFD1;
        statusFlag_ = 68;
    }

    ap_config::~ap_config()
    {
    }
}
