package com.medialab.airplay;

public interface IAirPlayHandler {

    /**
     * Represents the PCM audio data format.
     */
    static final int PCM = 0;

    /**
     * Represents the ALAC audio data format.
     */
    static final int ALAC = 1;

    /**
     * Represents the AAC audio data format.
     */
    static final int AAC = 2;

    /**
     * Represents the AAC-ELD audio data format.
     */
    static final int AACELD = 3;

    /**
     * Gets called when the mirroring stream starting.
     * @param session The id of the mirroring session.
     */
    void on_mirror_stream_started(String session);

    /**
     * Gets called when the mirroring video data codec to be set.
     * @param codec The mirroring video data codec.
     */
    void on_mirror_stream_codec(MirroringVideoCodec codec);

    /**
     * Gets called when the mirroring video data is coming.
     * @param data The mirroring video data (Containing 1 or more frames data).
     * @param timestamp The timestamp of this video data.
     */
    void on_mirror_stream_data(byte[] data, long timestamp);

    /**
     * Gets called when the mirroring session stopped.
     * @param session The id of the mirroring session.
     */
    void on_mirror_stream_stopped(String session);

    /**
     * Gets called when the sender requires to set the volume.
     * @param session The id of the audio session.
     * @param ratio The ratio of the volume (1 ~ 100).
     * @param volume The raw data of the volume.
     */
    void on_audio_set_volume(String session, float ratio, float volume);

    /**
     * Gets called when the sender requires to set the progress.
     * @param session  The id of the audio session.
     * @param ratio The ratio of the prgoress (1 ~ 100).
     * @param start The raw data or the start time.
     * @param current The raw data of the current position.
     * @param end The raw data of the end time.
     */
    void on_audio_set_progress(String session, float ratio, long start, long current, long end);

    /**
     * Gets called when the sender requires to set the cover image.
     * @param session The id of the audio session.
     * @param format The image format of the cover image.
     * @param data The binary raw data of the cover image.
     */
    void on_audio_set_cover(String session, String format, byte[] data);

    /**
     * Gets called when the sender requires to set the meta data.
     * Refer to the DAAP (http://tapjam.net/daap/).
     * @param session The id of the audio session.
     * @param data The raw binary raw meta data.
     */
    void on_audio_set_meta_data(String session, byte[] data);

    /**
     * Gets called when the audio stream session starting.
     * @param session The id of the audio session.
     * @param format The audio data format.
     */
    void on_audio_stream_started(String session, int format);

    /**
     * Gets called when the audio data coming.
     * @param data The audio data.
     */
    void on_audio_stream_data(byte[] data);

    // void on_audio_control_sync(AudioControlSync sync);

    // void on_audio_control_retransmit(AudioControlRetransmit retransmit);

    /**
     * Gets called when the audio stream session stopped.
     * @param session The id of the audio session.
     */
    void on_audio_stream_stopped(String session);

    /**
     * Gets called when the video stream session starting.
     * @param session The id of the video stream session.
     * @param location The video location (URI).
     * @param position The start position in milliseconds.
     */
    void on_video_play(String session, String location, float position);

    /**
     * Gets called when the sender requires to set the position of the video.
     * @param session The id of the video stream session.
     * @param position The required position in seconds.
     */
    void on_video_scrub(String session, float position);

    /**
     * Gets called when the sender requires to set the playback speed.
     * @param session The id of the video stream session.
     * @param value The speed required (0 ~ 1). 0 means the playback is paused, otherwise playing.
     */
    void on_video_rate(String session, float value);

    /**
     * Gets called when the video stream session stopped.
     * @param session The id of the video stream session.
     */
    void on_video_stop(String session);

    /**
     * Gets called when the sender requires the playback info.
     * @param session The id of the video stream session.
     * @return The {@link PlaybackInfo}.
     */
    PlaybackInfo get_playback_info(String session);
}
