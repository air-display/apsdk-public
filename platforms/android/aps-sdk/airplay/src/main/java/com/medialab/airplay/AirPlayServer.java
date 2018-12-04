package com.medialab.airplay;

import android.content.Context;
import android.util.Log;

import com.medialab.nci.NciObject;

public class AirPlayServer extends NciObject {
    private static final String TAG = "AirPlayServer";

    /**
     * Creates the native class instance.
     * @return the handle of the native object which is the value of the memory address.
     *
     */
    @Override
    protected long newNci() {
        // Call JNI method
        return nciNew();
    }
    private native long nciNew();

    /**
     * Destroys the native class instance.
     */
    @Override
    protected void deleteNci() {
        // Call JIN method
        nciDelete();
    }
    private native void nciDelete();

    private native void nciSetConfig(AirPlayConfig config);
    private native void nciSetHandler(AirPlayHandler handler);
    private native boolean nciStart();
    private native void nciStop();
    private native short nciGetServicePort();

    private AirPlayConfig config;

    private AirPlayHandler handler;

    public AirPlayServer(Context context) {
        MDNSHelper.initializeContext(context);
    }

    public AirPlayConfig getConfig() {
        return config;
    }

    public void setConfig(AirPlayConfig config) {
        this.config = config;
        nciSetConfig(config);
    }

    public AirPlayHandler getHandler() {
        return handler;
    }

    public void setHandler(AirPlayHandler handler) {
        this.handler = handler;
        nciSetHandler(handler);
    }

    public boolean start() {
        MDNSHelper.acquireMDNSDaemon();
        return nciStart();
    }

    public void stop() {
        MDNSHelper.releaseMDNSDaemon();
        nciStop();
    }

    public short getServicePort() {
        return nciGetServicePort();
    }
}
