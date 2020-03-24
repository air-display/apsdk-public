package com.virtable.airplay;

public class AirPlayConfigAudioFormat {
  private int type = 0;

  private int audioInputFormats = 0;

  private int audioOutputFormats = 0;

  public int getType() { return type; }

  public void setType(int type) { this.type = type; }

  public int getAudioInputFormats() { return audioInputFormats; }

  public void setAudioInputFormats(int audioInputFormats) {
    this.audioInputFormats = audioInputFormats;
  }

  public int getAudioOutputFormats() { return audioOutputFormats; }

  public void setAudioOutputFormats(int audioOutputFormats) {
    this.audioOutputFormats = audioOutputFormats;
  }
}
