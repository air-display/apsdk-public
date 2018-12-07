package com.medialab.airplay;

import android.content.Context;
import android.util.Log;

import com.medialab.nci.NciObject;

/**
 * Represents the AiaPlayer server.
 */
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

    /**
     * Constructs a instance of {@link AirPlayServer}.
     * @param context The context.
     */
    public AirPlayServer(Context context) {
        MDNSHelper.initializeContext(context);
    }

    /**
     * Gets the configuration data.
     * @return The {@link AirPlayConfig}.
     */
    public AirPlayConfig getConfig() {
        return config;
    }

    /**
     * Sets the configuration data.
     * @param config The {@link AirPlayConfig}.
     */
    public void setConfig(AirPlayConfig config) {
        this.config = config;
        nciSetConfig(config);
    }

    /**
     * Gets the event handler.
     * @return The {@link AirPlayHandler}.
     */
    public AirPlayHandler getHandler() {
        return handler;
    }

    /**
     * Sets the event handler.
     * @param handler The {@link AirPlayHandler}.
     */
    public void setHandler(AirPlayHandler handler) {
        this.handler = handler;
        nciSetHandler(handler);
    }

    /**
     * Starts the server.
     * @return True if successful; otherwise false.
     */
    public boolean start() {
        MDNSHelper.acquireMDNSDaemon();
        return nciStart();
    }

    /**
     * Stops the server.
     */
    public void stop() {
        MDNSHelper.releaseMDNSDaemon();
        nciStop();
    }

    /**
     * Gets the primary server port.
     * @return The port of the service.
     */
    public short getServicePort() {
        return nciGetServicePort();
    }
}
