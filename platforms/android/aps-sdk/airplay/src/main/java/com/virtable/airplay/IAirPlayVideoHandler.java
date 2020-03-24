package com.virtable.airplay;

public interface IAirPlayVideoHandler {
  /**
   * Gets called when the video stream session starting.
   *
   * @param session  The id of the video stream session.
   * @param location The video location (URI).
   * @param position The start position in milliseconds.
   */
  void on_video_play(long session, String location, float position);

  /**
   * Gets called when the sender requires to set the position of the video.
   *
   * @param session  The id of the video stream session.
   * @param position The required position in seconds.
   */
  void on_video_scrub(long session, float position);

  /**
   * Gets called when the sender requires to set the playback speed.
   *
   * @param session The id of the video stream session.
   * @param value   The speed required (0 ~ 1). 0 means the playback is paused, otherwise playing.
   */
  void on_video_rate(long session, float value);

  /**
   * Gets called when the video stream session stopped.
   *
   * @param session The id of the video stream session.
   */
  void on_video_stop(long session);

  /**
   * Gets called when the sender requires the playback info.
   *
   * @param session The id of the video stream session.
   * @return The {@link PlaybackInfo}.
   */
  PlaybackInfo get_playback_info(long session);
}
