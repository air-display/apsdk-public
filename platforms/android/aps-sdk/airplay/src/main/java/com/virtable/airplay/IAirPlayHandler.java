package com.virtable.airplay;

public interface IAirPlayHandler {
  /**
   * @param session The session is beginning.
   */
  void on_session_begin(AirPlaySession session);

  /**
   * @param session_id The session is ending.
   */
  void on_session_end(long session_id);
}
