package com.virtable.airplay;

public class AirPlayConfigDisplay {
  private int width = 0;

  private int height = 0;

  private float refreshRate = 0;

  private String uuid;

  public int getWidth() {
    return width;
  }

  public void setWidth(int width) {
    this.width = width;
  }

  public int getHeight() {
    return height;
  }

  public void setHeight(int height) {
    this.height = height;
  }

  public float getRefreshRate() {
    return refreshRate;
  }

  public void setRefreshRate(float refreshRate) {
    this.refreshRate = refreshRate;
  }

  public String getUuid() {
    return uuid;
  }

  public void setUuid(String uuid) {
    this.uuid = uuid;
  }
}
