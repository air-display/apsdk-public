#pragma once
#include <cmath>
#include <cstdint>
#include <string>

#define DECLARE_STRING_PROPERTY(n) private: std::string n##_; \
public: const std::string& n() const { return n##_; } \
public: void n(const std::string& value) { n##_= value; }

#define DECLARE_INTEGER_PROPERTY(n) private: std::int32_t n##_; \
public: const std::int32_t& n() const { return n##_; } \
public: void n(const std::int32_t& value) { n##_= value; }

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

        DECLARE_INTEGER_PROPERTY(audioInputFormats);
        DECLARE_INTEGER_PROPERTY(audioOutputFormats);
        DECLARE_INTEGER_PROPERTY(type);
    };

    class ap_config_audio_latency
    {
    public:
        ap_config_audio_latency();
        ~ap_config_audio_latency();

        DECLARE_STRING_PROPERTY(audioType);
        DECLARE_INTEGER_PROPERTY(inputLatencyMicros);
        DECLARE_INTEGER_PROPERTY(outputLatencyMicros);
        DECLARE_INTEGER_PROPERTY(type);
    };
    
    class ap_config_display
    {
    public:
        ap_config_display();
        ~ap_config_display();

        DECLARE_INTEGER_PROPERTY(width);
        DECLARE_INTEGER_PROPERTY(height);
        DECLARE_FLOAT_PROPERTY(refreshRate);
        DECLARE_STRING_PROPERTY(uuid);
    };

    class ap_config
    {
    public:
        static ap_config& get();
        void init();

        DECLARE_STRING_PROPERTY(name);
        DECLARE_STRING_PROPERTY(deviceID);
        DECLARE_STRING_PROPERTY(model);
        DECLARE_STRING_PROPERTY(sourceVersion);
        DECLARE_STRING_PROPERTY(pi);
        DECLARE_STRING_PROPERTY(pk);
        DECLARE_STRING_PROPERTY(macAddress);
        DECLARE_INTEGER_PROPERTY(vv);
        DECLARE_INTEGER_PROPERTY(features);
        DECLARE_INTEGER_PROPERTY(statusFlag);

        DECLARE_OBJECT_PROPERTY(audio_format, ap_config_audio_format);
        DECLARE_OBJECT_PROPERTY(audio_latency, ap_config_audio_latency);
        DECLARE_OBJECT_PROPERTY(display, ap_config_display);

    protected:
        ap_config();
        ~ap_config();
    };
};

