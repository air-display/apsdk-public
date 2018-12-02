package com.medialab.airplay;

import android.print.PrinterId;

public interface IAirPlayHandler {
    //
    static final int PCM = 0;
    static final int ALAC = 1;
    static final int AAC = 2;
    static final int AACELD = 3;

    // Mirroring
    void on_mirror_stream_started();

    void on_mirror_stream_codec(MirroringVideoCodec codec);

    void on_mirror_stream_data(byte[] data, long timestamp);

    void on_mirror_stream_stopped();

    // Audio
    void on_audio_set_volume(float ratio, float volume);

    void on_audio_set_progress(float ratio, long start, long current, long end);

    void on_audio_set_cover(String format, byte[] data);

    void on_audio_set_meta_data(byte[] data);

    void on_audio_stream_started(int format);

    void on_audio_stream_data(byte[] data);

    // void on_audio_control_sync(AudioControlSync sync);

    // void on_audio_control_retransmit(AudioControlRetransmit retransmit);

    void on_audio_stream_stopped();

    // Video
    void on_video_play(String location, float start_pos);

    void on_video_scrub(float position);

    void on_video_rate(float value);

    void on_video_stop();

    PlaybackInfo get_playback_info();
}
