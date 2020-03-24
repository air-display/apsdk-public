package com.virtable.airplay;

public class AirPlayConfigAudioLatency {

  private int type = 0;

  private String audioType;

  private int inputLatencyMicros = 0;

  private int outputLatencyMicros = 0;

  public int getType() { return type; }

  public void setType(int type) { this.type = type; }

  public String getAudioType() { return audioType; }

  public void setAudioType(String audioType) { this.audioType = audioType; }

  public int getInputLatencyMicros() { return inputLatencyMicros; }

  public void setInputLatencyMicros(int inputLatencyMicros) {
    this.inputLatencyMicros = inputLatencyMicros;
  }

  public int getOutputLatencyMicros() { return outputLatencyMicros; }

  public void setOutputLatencyMicros(int outputLatencyMicros) {
    this.outputLatencyMicros = outputLatencyMicros;
  }
}
