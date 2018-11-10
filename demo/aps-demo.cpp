// aps-demo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "../src/ap_server.h"
#include "../src/ap_config.h"
#include "../src/utils/logger.h"

class ap_player : public aps::ap_handler
{
public:
    ap_player();
    ~ap_player();

    virtual void on_mirror_stream_started() override
    {
        LOGI() << "on_mirror_stream_started";
    }


    virtual void on_mirror_stream_data(const void* data) override
    {
        LOGI() << "on_mirror_stream_data: " << data;
    }


    virtual void on_mirror_stream_stopped() override
    {
        LOGI() << "on_mirror_stream_stopped";
    }


    virtual void on_audio_set_volume(const float ratio, const float volume) override
    {
        LOGI() << "on_audio_set_volume: " << ratio
            << ", value: " << volume;
    }


    virtual void on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current, const uint64_t end) override
    {
        LOGI() << "on_audio_set_progress: " << ratio
            << ", start: " << start
            << ", current: " << current
            << ", end: " << end;
    }


    virtual void on_audio_set_cover(const std::string format, const void* data, const uint32_t length) override
    {
        LOGI() << "on_audio_set_cover: " << format;
    }


    virtual void on_audio_set_meta_data(const void* data, const uint32_t length) override
    {
        LOGI() << "on_audio_set_meta_data: " << data;
    }


    virtual void on_audio_stream_started() override
    {
        LOGI() << "on_audio_stream_started: ";
    }


    virtual void on_audio_stream_data(const void* data) override
    {
        LOGI() << "on_audio_stream_data: " << data;
    }


    virtual void on_audio_stream_stopped() override
    {
        LOGI() << "on_audio_stream_stopped";
    }


    virtual void on_video_play(const std::string& location, const float start_pos) override
    {
        LOGI() << "on_video_play: " << location;
    }


    virtual void on_video_scrub(const float postion) override
    {
        LOGI() << "on_video_scrub: " << postion;
    }


    virtual void on_video_rate(const float value) override
    {
        LOGI() << "on_video_rate: " << value;
    }


    virtual void on_video_stop() override
    {
        LOGI() << "on_video_stop: ";
    }


    virtual void on_acquire_playback_info(playback_info_t& playback_info) override
    {
        LOGI() << "on_acquire_playback_info: ";
    }
};

ap_player::ap_player()
{
}

ap_player::~ap_player()
{
}

int main()
{
    aps::ap_handler_ptr player = std::make_shared<ap_player>();
    aps::ap_server server(aps::ap_config::default_instance());
    server.set_handler(player);
    LOGI() << "AP Server is starting....";
    server.start();
    LOGI() << "AP Server started....";
    getchar();
    server.stop();
}
