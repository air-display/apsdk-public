package com.virtable.airplay;

public interface IAirPlayMirrorHandler {
  /**
   * Represents the PCM audio data format.
   */
  int PCM = 0;

  /**
   * Represents the ALAC audio data format.
   */
  int ALAC = 1;

  /**
   * Represents the AAC audio data format.
   */
  int AAC = 2;

  /**
   * Represents the AAC-ELD audio data format.
   */
  int AACELD = 3;

  /**
   * Gets called when the mirroring stream starting.
   */
  void on_mirror_stream_started();

  /**
   * Gets called when the mirroring video data codec to be set.
   */
  void on_mirror_stream_codec(byte[] data);

  /**
   * Gets called when the mirroring video data is coming.
   *
   * @param data      The mirroring video data (Containing 1 or more frames data).
   * @param timestamp The timestamp of this video data.
   */
  void on_mirror_stream_data(byte[] data, long timestamp);

  /**
   * Gets called when the mirroring video heartbeat is coming.
   *
   */
  void on_mirror_stream_heartbeat();

  /**
   * Gets called when the mirroring session stopped.
   */
  void on_mirror_stream_stopped();

  /**
   * Gets called when the sender requires to set the volume.
   *
   * @param ratio  The ratio of the volume (1 ~ 100).
   * @param volume The raw data of the volume.
   */
  void on_audio_set_volume(float ratio, float volume);

  /**
   * Gets called when the sender requires to set the progress.
   *
   * @param ratio   The ratio of the prgoress (1 ~ 100).
   * @param start   The raw data or the start time.
   * @param current The raw data of the current position.
   * @param end     The raw data of the end time.
   */
  void on_audio_set_progress(float ratio, long start, long current, long end);

  /**
   * Gets called when the sender requires to set the cover image.
   *
   * @param format The image format of the cover image.
   * @param data   The binary raw data of the cover image.
   */
  void on_audio_set_cover(String format, byte[] data);

  /**
   * Gets called when the sender requires to set the meta data.
   * Refer to the DAAP (http://tapjam.net/daap/).
   *
   * @param data The raw binary raw meta data.
   */
  void on_audio_set_meta_data(byte[] data);

  /**
   * Gets called when the audio stream session starting.
   *
   * @param format The audio data format.
   */
  void on_audio_stream_started(int format);

  /**
   * Gets called when the audio data coming.
   *
   * @param data The audio data.
   */
  void on_audio_stream_data(byte[] data, long timestamp);

  // void on_audio_control_sync(AudioControlSync sync);

  // void on_audio_control_retransmit(AudioControlRetransmit retransmit);

  /**
   * Gets called when the audio stream session stopped.
   */
  void on_audio_stream_stopped();
}
