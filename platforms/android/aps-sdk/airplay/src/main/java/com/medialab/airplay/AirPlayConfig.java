package com.medialab.airplay;

public class AirPlayConfig {
    private static AirPlayConfig s_instance = null;

    public static AirPlayConfig defaultInstance() {
        if (s_instance == null) {
            s_instance = new AirPlayConfig();
            s_instance.setName("Tencent WeCast Display");
            s_instance.setDeviceID("00:00:00:00:00:00");
            s_instance.setModel("AppleTV3,2");
            s_instance.setSourceVersion("220.68");
            s_instance.setPi("b08f5a79-db29-4384-b456-a4784d9e6055");
            s_instance.setPk("99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63");
            s_instance.setMacAddress(s_instance.getDeviceID());
            s_instance.setVv(2);
            s_instance.setFeatures(0x5A7FDFD1);
            s_instance.setStatusFlag(68);

            s_instance.display.setWidth(1920);
            s_instance.display.setHeight(1080);
            s_instance.display.setRefreshRate(1.0f / 24);
            s_instance.display.setUuid("e5f7a68d-7b0f-4305-984b-974f677a150b");

            s_instance.audioLatency.setType(96);
            s_instance.audioLatency.setAudioType("default");
            s_instance.audioLatency.setInputLatencyMicros(3);
            s_instance.audioLatency.setOutputLatencyMicros(79);

            s_instance.audioFormat.setType(96);
            s_instance.audioFormat.setAudioInputFormats(0x01000000);
            s_instance.audioFormat.setAudioOutputFormats(0x01000000);
        }

        return s_instance;
    }

    private String name;

    private String deviceID;

    private String model;

    private String sourceVersion;

    private String pi;

    private String pk;

    private String macAddress;

    private int vv = 0;

    private int features = 0;

    private int statusFlag = 0;

    private AirPlayConfigAudioFormat audioFormat = new AirPlayConfigAudioFormat();

    private AirPlayConfigAudioLatency audioLatency = new AirPlayConfigAudioLatency();

    private AirPlayConfigDisplay display = new AirPlayConfigDisplay();

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDeviceID() {
        return deviceID;
    }

    public void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getSourceVersion() {
        return sourceVersion;
    }

    public void setSourceVersion(String sourceVersion) {
        this.sourceVersion = sourceVersion;
    }

    public String getPi() {
        return pi;
    }

    public void setPi(String pi) {
        this.pi = pi;
    }

    public String getPk() {
        return pk;
    }

    public void setPk(String pk) {
        this.pk = pk;
    }

    public String getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(String macAddress) {
        this.macAddress = macAddress;
    }

    public int getVv() {
        return vv;
    }

    public void setVv(int vv) {
        this.vv = vv;
    }

    public String getFeaturesString() {
        return Integer.toHexString(features);
    }

    public int getFeatures() {
        return features;
    }

    public void setFeatures(int features) {
        this.features = features;
    }

    public int getStatusFlag() {
        return statusFlag;
    }

    public void setStatusFlag(int statusFlag) {
        this.statusFlag = statusFlag;
    }

    public AirPlayConfigAudioFormat getAudioFormat() {
        return audioFormat;
    }

    public void setAudioFormat(AirPlayConfigAudioFormat audioFormat) {
        this.audioFormat = audioFormat;
    }

    public AirPlayConfigAudioLatency getAudioLatency() {
        return audioLatency;
    }

    public void setAudioLatency(AirPlayConfigAudioLatency audioLatency) {
        this.audioLatency = audioLatency;
    }

    public AirPlayConfigDisplay getDisplay() {
        return display;
    }

    public void setDisplay(AirPlayConfigDisplay display) {
        this.display = display;
    }
}
