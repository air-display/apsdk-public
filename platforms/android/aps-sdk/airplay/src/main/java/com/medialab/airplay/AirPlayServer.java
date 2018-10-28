package com.medialab.airplay;

public class AirPlayServer {
    static {
        System.loadLibrary("aps-jni");
    }

    public native boolean initialize();

    public native boolean start();

    public native void stop();

    public native void uninitialize();
}
