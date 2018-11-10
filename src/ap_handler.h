#ifndef AP_HANDLER_H
#define AP_HANDLER_H
#pragma once
#include <vector>
#include <memory>

namespace aps {
    class ap_handler
    {
    public:
        struct time_range_s
        {
            double start;
            double duration;
        };
        typedef time_range_s time_range_t;

        typedef std::vector<time_range_t> time_range_array;

        struct playback_info_s
        {
            std::string uuid;
            uint32_t stallCount;
            double duration;
            float position;
            double rate;
            bool readyToPlay;
            bool playbackBufferEmpty;
            bool playbackBufferFull;
            bool playbackLikelyToKeepUp;
            time_range_array loadedTimeRanges;
            time_range_array seekableTimeRanges;
        };
        typedef playback_info_s playback_info_t;

        virtual ~ap_handler() {};

        // Mirroring
        virtual void on_mirror_stream_started() = 0;

        virtual void on_mirror_stream_data(
            const void* data) = 0;
        
        virtual void on_mirror_stream_stopped() = 0;
        
        // Audio
        virtual void on_audio_set_volume(
            const float ratio,
            const float volume) = 0;
        
        virtual void on_audio_set_progress(
            const float ratio,
            const uint64_t start,
            const uint64_t current,
            const uint64_t end) = 0;
        
        virtual void on_audio_set_cover(
            const std::string format,
            const void* data,
            const uint32_t length) = 0;

        virtual void on_audio_set_meta_data(
            const void* data,
            const uint32_t length) = 0;

        virtual void on_audio_stream_started() = 0;

        virtual void on_audio_stream_data(
            const void* data) = 0;

        virtual void on_audio_stream_stopped() = 0;

        // Video
        virtual void on_video_play(
            const std::string& location,
            const float start_pos) = 0;
        
        virtual void on_video_scrub(
            const float postion) = 0;

        virtual void on_video_rate(
            const float value) = 0;

        virtual void on_video_stop() = 0;

        virtual void on_acquire_playback_info(
            playback_info_t& playback_info) = 0;
    };

    typedef std::shared_ptr<ap_handler> ap_handler_ptr;
}
#endif // AP_HANDLER_H