/* 
 *  File: PlaybackInfo.java
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

package com.sheentech.apsdk;

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
