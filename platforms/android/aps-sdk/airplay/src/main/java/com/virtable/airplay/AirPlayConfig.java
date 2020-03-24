package com.virtable.airplay;

/**
 * Represents the configuration data of the AirPlay server.
 */
public class AirPlayConfig {
  private String name;
  private static String randomMac = null;
  private boolean publishService;
  private String model;
  private String sourceVersion;
  private String pi;
  private String pk;
  private String macAddress;
  private int vv = 0;
  private int features = 0;
  private int statusFlag = 0;
  private AirPlayConfigAudioFormat audioFormat = new AirPlayConfigAudioFormat();
  private AirPlayConfigAudioLatency audioLatency =
      new AirPlayConfigAudioLatency();
  private AirPlayConfigDisplay display = new AirPlayConfigDisplay();
  public static AirPlayConfig defaultInstance() {
    AirPlayConfig instance = new AirPlayConfig();
    instance.setName("Virtalbe Airplay Display");
    instance.publishService = true;
    instance.setMacAddress(instance.generateMacAddress());
    instance.setModel("AppleTV3,2");
    instance.setSourceVersion("220.68");
    instance.setPi("b08f5a79-db29-4384-b456-a4784d9e6055");
    instance.setPk(
        "99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63");
    instance.setVv(2);
    instance.setFeatures(0x527FFFF7);
    instance.setStatusFlag(68);
    instance.display.setWidth(1920);
    instance.display.setHeight(1080);
    instance.display.setRefreshRate(1.0f / 24);
    instance.display.setUuid("e5f7a68d-7b0f-4305-984b-974f677a150b");
    instance.audioLatency.setType(96);
    instance.audioLatency.setAudioType("default");
    instance.audioLatency.setInputLatencyMicros(3);
    instance.audioLatency.setOutputLatencyMicros(79);
    instance.audioFormat.setType(96);
    instance.audioFormat.setAudioInputFormats(0x01000000);
    instance.audioFormat.setAudioOutputFormats(0x01000000);
    return instance;
  }
  public String getName() { return name; }
  public void setName(String name) { this.name = name; }

  public boolean isPublishService() { return publishService; }
  public void setPublishService(boolean publishService) { this.publishService = publishService; }

  public String getMacAddress() { return macAddress; }
  public void setMacAddress(String macAddress) { this.macAddress = macAddress; }

  public int getVv() { return vv; }
  private void setVv(int vv) { this.vv = vv; }
  public int getFeatures() { return features; }
  private void setFeatures(int features) { this.features = features; }
  public int getStatusFlag() { return statusFlag; }
  private void setStatusFlag(int statusFlag) { this.statusFlag = statusFlag; }
  public String getDeviceID() { return simplifyMacAddress(macAddress); }
  public String getModel() { return model; }
  ///////////////////////////////////////////////////////////////////////////////////////
  private void setModel(String model) { this.model = model; }
  public String getSourceVersion() { return sourceVersion; }
  private void setSourceVersion(String sourceVersion) {
    this.sourceVersion = sourceVersion;
  }
  public String getPi() { return pi; }
  private void setPi(String pi) { this.pi = pi; }
  public String getPk() { return pk; }
  private void setPk(String pk) { this.pk = pk; }
  public String getFeaturesString() { return Integer.toHexString(features); }
  public AirPlayConfigAudioFormat getAudioFormat() { return audioFormat; }
  private void setAudioFormat(AirPlayConfigAudioFormat audioFormat) {
    this.audioFormat = audioFormat;
  }
  public AirPlayConfigAudioLatency getAudioLatency() { return audioLatency; }
  private void setAudioLatency(AirPlayConfigAudioLatency audioLatency) {
    this.audioLatency = audioLatency;
  }
  public AirPlayConfigDisplay getDisplay() { return display; }
  private void setDisplay(AirPlayConfigDisplay display) {
    this.display = display;
  }

  private String generateMacAddress() {
    if (null != randomMac)
      return randomMac;

    long ts = System.currentTimeMillis();
    randomMac = String.format("%02X:%02X:%02X:%02X:%02X:%02X",
        (ts & 0xff), ((ts >> 8) & 0xff),
        ((ts >> 16) & 0xff), ((ts >> 24) & 0xff),
        ((ts >> 32) & 0xff), ((ts >> 40) & 0xff));
    return randomMac;
  }

  private String simplifyMacAddress(String macAddress) {
    return macAddress.replace(":", "");
  }
}
