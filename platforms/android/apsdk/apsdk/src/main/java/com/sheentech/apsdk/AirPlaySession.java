/* 
 *  File: AirPlaySession.java
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

import android.os.ConditionVariable;

/**
 *
 */
public class AirPlaySession extends NciObject {
  public static final int MIRROR_SESSION = 0;
  public static final int VIDEO_SESSION = 1;
  public static final int UNKNOWN_SESSION = 0xffffffff;

  /**
   * Creates the native class instance.
   */
  @Override
  protected void newNci() { }

  /**
   * Destroys the native class instance.
   */
  private native void nciDelete();
  @Override
  protected void deleteNci() {
    // Call JIN method
    nciDelete();
  }

  /**
   * @return The session id.
   */
  private native long nciGetSessionId();
  public long getSessionId() {
    return nciGetSessionId();
  }

  /**
   * @return The session type.
   */
  private native int nciGetSessionType();
  public int getSessionType() {
    return nciGetSessionType();
  }

  /**
   * @param handler The mirror session handler.
   */
  private native void nciSetMirrorHandler(IAirPlayMirroringHandler handler);
  public void setMirrorHandler(IAirPlayMirroringHandler handler) {
    nciSetMirrorHandler(handler);
  }

  /**
   * @param handler The video stream session handler.
   */
  private native void nciSetCastHandler(IAirPlayCastingHandler handler);
  public void setCastHandler(IAirPlayCastingHandler handler) {
    nciSetCastHandler(handler);
  }

  /**
   *
   */
  private native void nciDisconnect();
  public void disconnect() {
    nciDisconnect();
  }

  // The callback interface
  public interface StopHandler {
    void onSessionStop();
  }

  private StopHandler mStopHandler;
  public void setStopHandler(StopHandler handler) {
    mStopHandler = handler;
  }

  public void stopSession() {
    if (null != mStopHandler) {
      mStopHandler.onSessionStop();
    }
  }

  private ConditionVariable mCondiationVariable = new ConditionVariable();
  public void waitForReady() {
    mCondiationVariable.block();
  }

  public void notifyReady() {
    mCondiationVariable.open();
  }

  public void resetReady() {
    mCondiationVariable.close();
  }
}
