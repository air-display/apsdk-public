package com.sheensoftlab.apsdk;

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
}
