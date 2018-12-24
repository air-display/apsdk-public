package com.medialab.airplay;

public interface IAirPlayHandler {
  /**
   * @param session
   */
  void on_session_begin(AirPlaySession session);

  /**
   * @param session_id
   */
  void on_session_end(long session_id);
}
