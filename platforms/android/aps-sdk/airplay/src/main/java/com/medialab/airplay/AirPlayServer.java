package com.medialab.airplay;

import android.content.Context;

public class AirPlayServer {
    static {
        System.loadLibrary("aps-jni");
    }

    private Context context;

    private AirPlayConfig config;

    private AirPlayHandler handler;

    public AirPlayServer(Context context, AirPlayConfig config) {
        this.context = context;
        this.config = config;
    }

    public Context getContext() {
        return context;
    }

    public void setContext(Context context) {
        this.context = context;
    }

    public AirPlayConfig getConfig() {
        return config;
    }

    public void setConfig(AirPlayConfig config) {
        this.config = config;
    }

    public AirPlayHandler getHandler() {
        return handler;
    }

    public void setHandler(AirPlayHandler handler) {
        this.handler = handler;
    }

    public native boolean start();

    public native void stop();
}
