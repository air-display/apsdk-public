package com.medialab.airplay;

import java.nio.ByteBuffer;

public interface IAirPlayHandler {
    // Mirroring
    void on_mirror_stream_started();

    void on_mirror_stream_data(ByteBuffer data);

    void on_mirror_stream_stopped();

    // Audio
    void on_audio_set_volume(float ratio, float volume);

    void on_audio_set_progress(float ratio, long start, long current, long end);

    void on_audio_set_cover(String format, ByteBuffer data, long length);

    void on_audio_set_meta_data(ByteBuffer data, long length);

    void on_audio_stream_started();

    void on_audio_stream_data(ByteBuffer data);

    void on_audio_stream_stopped();

    // Video
    void on_video_play(String location, float start_pos);

    void on_video_scrub(float position);

    void on_video_rate(float value);

    void on_video_stop();

    //void on_acquire_playback_info(playback_info_t playback_info);
}
