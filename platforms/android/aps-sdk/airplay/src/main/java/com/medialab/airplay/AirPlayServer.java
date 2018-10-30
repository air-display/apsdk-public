package com.medialab.airplay;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.Build;

public class AirPlayServer {
    static {
        System.loadLibrary("aps-jni");
    }

    private NsdManager.RegistrationListener mRegistrationListener = null;

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

    private void acquireMdnsd() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            NsdServiceInfo serviceInfo = new NsdServiceInfo();
            serviceInfo.setServiceName("o");
            serviceInfo.setServiceType("_x._tcp");
            serviceInfo.setPort(1);
            mRegistrationListener = new NsdManager.RegistrationListener() {
                @Override
                public void onServiceRegistered(NsdServiceInfo NsdServiceInfo) { }
                @Override
                public void onRegistrationFailed(NsdServiceInfo serviceInfo, int errorCode) { }
                @Override
                public void onServiceUnregistered(NsdServiceInfo arg0) { }
                @Override
                public void onUnregistrationFailed(NsdServiceInfo serviceInfo, int errorCode) { }
            };
            NsdManager nsdManager = (NsdManager) context.getSystemService(context.NSD_SERVICE);
            nsdManager.registerService(serviceInfo, NsdManager.PROTOCOL_DNS_SD, mRegistrationListener);
        }
    }

    private void releaseMdnsd() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            if (null != mRegistrationListener) {
                NsdManager nsdManager = (NsdManager) context.getSystemService(context.NSD_SERVICE);
                nsdManager.unregisterService(mRegistrationListener);
            }
        }
    }
}
