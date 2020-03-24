package com.virtable.airplay;

import android.content.Context;

/**
 * Represents the AiaPlayer server.
 */
public class AirPlayServer extends NciObject {
  private static final String TAG = "AirPlayServer";
  private AirPlayConfig config;
  private IAirPlayHandler handler;

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
    this.handler = handler;
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
