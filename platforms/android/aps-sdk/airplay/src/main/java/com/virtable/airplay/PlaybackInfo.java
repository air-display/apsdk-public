package com.virtable.airplay;

/**
 * Represents the playback info.
 */
public class PlaybackInfo {
  /**
   * Currently not used.
   */
  public String uuid;

  /**
   * Currently not used.
   */
  public int stallCount;

  /**
   * The duration in seconds of the video.
   */
  public double duration;

  /**
   * The position in seconds of the video.
   */
  public float position;

  /**
   * The playback speed (0 ~ 1). 0 means paused.
   */
  public double rate;

  /**
   * Currently not used.
   */
  public boolean readyToPlay;

  /**
   * Currently not used.
   */
  public boolean playbackBufferEmpty;

  /**
   * Currently not used.
   */
  public boolean playbackBufferFull;

  /**
   * Currently not used.
   */
  public boolean playbackLikelyToKeepUp;

  /**
   * Constructs the instance of the {@link PlaybackInfo}.
   */
  public PlaybackInfo() {
    uuid = "";
    stallCount = 0;
    duration = 0;
    position = 0;
    rate = 0;
    readyToPlay = false;
    playbackBufferEmpty = true;
    playbackBufferFull = false;
    playbackLikelyToKeepUp = false;
  }
}
