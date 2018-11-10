#ifndef AP_CONFIG_H
#define AP_CONFIG_H
#pragma once

#include <cmath>
#include <cstdint>
#include <string>

#define DECLARE_STRING_PROPERTY(n) private: std::string n##_; \
public: const std::string& n() const { return n##_; } \
public: void n(const std::string& value) { n##_= value; }

#define DECLARE_INTEGER32_PROPERTY(n) private: std::int32_t n##_; \
public: const std::int32_t& n() const { return n##_; } \
public: void n(const std::int32_t& value) { n##_= value; }

#define DECLARE_INTEGER64_PROPERTY(n) private: std::int64_t n##_; \
public: const std::int64_t& n() const { return n##_; } \
public: void n(const std::int64_t& value) { n##_= value; }

#define DECLARE_FLOAT_PROPERTY(n) private: std::float_t n##_; \
public: const std::float_t& n() const { return n##_; } \
public: void n(const std::float_t& value) { n##_= value; }

#define DECLARE_OBJECT_PROPERTY(n, t) private: t n##_; \
public: const t& n() const { return n##_; } \
public: void n(const t& value) { n##_= value; }

namespace aps {
    class ap_config_audio_format
    {
    public:
        ap_config_audio_format();
        ~ap_config_audio_format();

        DECLARE_INTEGER32_PROPERTY(audioInputFormats);
        DECLARE_INTEGER32_PROPERTY(audioOutputFormats);
        DECLARE_INTEGER32_PROPERTY(type);
    };

    class ap_config_audio_latency
    {
    public:
        ap_config_audio_latency();
        ~ap_config_audio_latency();

        DECLARE_STRING_PROPERTY(audioType);
        DECLARE_INTEGER32_PROPERTY(inputLatencyMicros);
        DECLARE_INTEGER32_PROPERTY(outputLatencyMicros);
        DECLARE_INTEGER32_PROPERTY(type);
    };
    
    class ap_config_display
    {
    public:
        ap_config_display();
        ~ap_config_display();

        DECLARE_INTEGER32_PROPERTY(width);
        DECLARE_INTEGER32_PROPERTY(height);
        DECLARE_FLOAT_PROPERTY(refreshRate);
        DECLARE_STRING_PROPERTY(uuid);
    };

    class ap_config
    {
    public:
        static ap_config& default_instance();

        ap_config();

        ~ap_config();

        DECLARE_STRING_PROPERTY(name);
        DECLARE_STRING_PROPERTY(deviceID);
        DECLARE_STRING_PROPERTY(model);
        DECLARE_STRING_PROPERTY(serverVersion);
        DECLARE_STRING_PROPERTY(macAddress);
        DECLARE_STRING_PROPERTY(pi);
        DECLARE_STRING_PROPERTY(pk);
        DECLARE_STRING_PROPERTY(flags);

        DECLARE_STRING_PROPERTY(audioCodecs);
        DECLARE_STRING_PROPERTY(encryptionTypes);
        DECLARE_STRING_PROPERTY(metadataTypes);
        DECLARE_STRING_PROPERTY(needPassword);
        DECLARE_STRING_PROPERTY(transmissionProtocol);

        DECLARE_INTEGER32_PROPERTY(vv);
        DECLARE_INTEGER64_PROPERTY(features);
        DECLARE_INTEGER32_PROPERTY(statusFlag);
        DECLARE_OBJECT_PROPERTY(audio_format, ap_config_audio_format);
        DECLARE_OBJECT_PROPERTY(audio_latency, ap_config_audio_latency);
        DECLARE_OBJECT_PROPERTY(display, ap_config_display);

        const std::string features_hex_string() const;
    };
};

#endif // AP_CONFIG_H
