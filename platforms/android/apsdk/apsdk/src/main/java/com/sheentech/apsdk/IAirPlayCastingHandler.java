/* 
 *  File: IAirPlayCastingHandler.java
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

public interface IAirPlayCastingHandler {
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
