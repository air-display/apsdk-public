/* 
 *  File: AirPlayServer.java
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

import android.content.Context;

/**
 * Represents the AiaPlayer server.
 */
public class AirPlayServer extends NciObject {
  private AirPlayConfig config;

  /**
   * Constructs a instance of {@link AirPlayServer}.
   *
   * @param context The context.
   */
  public AirPlayServer(Context context) {
    MDNSHelper.initializeContext(context);
  }
  /**
   * Creates the native class instance.
   */
  private native void nciNew();
  @Override
  protected void newNci() {
    // Call JNI method
    nciNew();
  }
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
   * Gets the configuration data.
   *
   * @return The {@link AirPlayConfig}.
   */
  public AirPlayConfig getConfig() {
    return config;
  }
  /**
   * Sets the configuration data.
   *
   * @param config The {@link AirPlayConfig}.
   */
  public void setConfig(AirPlayConfig config) {
    this.config = config;
    nciSetConfig(config);
  }
  private native void nciSetConfig(AirPlayConfig config);
  private native void nciSetHandler(IAirPlayHandler handler);
  /**
   * Sets the event handler.
   *
   * @param handler The {@link IAirPlayHandler}.
   */
  public void setHandler(IAirPlayHandler handler) {
    nciSetHandler(handler);
  }

  private native boolean nciStart();
  /**
   * Starts the server.
   *
   * @return True if successful; otherwise false.
   */
  public boolean start() {
    MDNSHelper.acquireMDNSDaemon();
    return nciStart();
  }

  private native void nciStop();
  /**
   * Stops the server.
   */
  public void stop() {
    MDNSHelper.releaseMDNSDaemon();
    nciStop();
  }

  private native int nciGetServicePort();
  /**
   * Gets the primary server port.
   *
   * @return The port of the service.
   */
  public int getServicePort() {
    return nciGetServicePort();
  }
}
