package com.medialab.airplay;

public class AirPlayConfig {
    public static AirPlayConfig defaultInstance() {
        AirPlayConfig instance = new AirPlayConfig();
        instance.setName("Tencent WeCast Display");
        instance.setMacAddress(instance.generateMacAddress());
        instance.setDeviceID(instance.simplifyMacAddress(instance.getMacAddress()));
        instance.setModel("AppleTV3,2");
        instance.setSourceVersion("220.68");
        instance.setPi("b08f5a79-db29-4384-b456-a4784d9e6055");
        instance.setPk("99FD4299889422515FBD27949E4E1E21B2AF50A454499E3D4BE75A4E0F55FE63");
        instance.setVv(2);
        instance.setFeatures(0x5A7FDFD1);
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

    private static String randomMac = null;

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

    private void setDeviceID(String deviceID) {
        this.deviceID = deviceID;
    }

    public String getModel() {
        return model;
    }

    private void setModel(String model) {
        this.model = model;
    }

    public String getSourceVersion() {
        return sourceVersion;
    }

    private void setSourceVersion(String sourceVersion) {
        this.sourceVersion = sourceVersion;
    }

    public String getPi() {
        return pi;
    }

    private void setPi(String pi) {
        this.pi = pi;
    }

    public String getPk() {
        return pk;
    }

    private void setPk(String pk) {
        this.pk = pk;
    }

    public String getMacAddress() {
        return macAddress;
    }

    private void setMacAddress(String macAddress) {
        this.macAddress = macAddress;
    }

    public int getVv() {
        return vv;
    }

    private void setVv(int vv) {
        this.vv = vv;
    }

    public String getFeaturesString() {
        return Integer.toHexString(features);
    }

    public int getFeatures() {
        return features;
    }

    private void setFeatures(int features) {
        this.features = features;
    }

    public int getStatusFlag() {
        return statusFlag;
    }

    private void setStatusFlag(int statusFlag) {
        this.statusFlag = statusFlag;
    }

    public AirPlayConfigAudioFormat getAudioFormat() {
        return audioFormat;
    }

    private void setAudioFormat(AirPlayConfigAudioFormat audioFormat) {
        this.audioFormat = audioFormat;
    }

    public AirPlayConfigAudioLatency getAudioLatency() {
        return audioLatency;
    }

    private void setAudioLatency(AirPlayConfigAudioLatency audioLatency) {
        this.audioLatency = audioLatency;
    }

    public AirPlayConfigDisplay getDisplay() {
        return display;
    }

    private void setDisplay(AirPlayConfigDisplay display) {
        this.display = display;
    }

    private String generateMacAddress() {
        if (null != randomMac)
            return randomMac;

        long ts = System.currentTimeMillis();
        randomMac = String.format(
                "%02X:%02X:%02X:%02X:%02X:%02X",
                ((ts >> 0) & 0xff), ((ts >> 8) & 0xff),
                ((ts >> 16) & 0xff), ((ts >> 24) & 0xff),
                ((ts >> 32) & 0xff), ((ts >> 40) & 0xff));
        return randomMac;
    }

    private String simplifyMacAddress(String macAddress) {
        return macAddress.replace(":", "");
    }
}
