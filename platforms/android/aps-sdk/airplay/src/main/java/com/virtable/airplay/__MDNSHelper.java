package com.virtable.airplay;

import android.content.Context;
import android.net.nsd.NsdManager;
import android.net.nsd.NsdServiceInfo;
import android.os.Build;
import android.util.Log;

final class MDNSHelper {
  /**
   * Log tag.
   */
  private static final String TAG = "MDNSHelper";

  /**
   * The application context.
   */
  static private Context mContext = null;

  /**
   * The service registration listener.
   */
  static private NsdManager.RegistrationListener mRegistrationListener = null;

  /**
   * Initializes the application context.
   *
   * @param context The context.
   */
  static void initializeContext(Context context) {
    if (null == mContext) {
      mContext = context.getApplicationContext();
    }
  }

  /**
   * Launches the mdns daemon process by registering an empty service.
   */
  static void acquireMDNSDaemon() {
    if (null != mRegistrationListener) {
      Log.i(TAG, "acquireMDNSDaemon: MDNS Daemon is running already.");
      return;
    }

    if (null == mContext) {
      Log.e(TAG, "acquireMDNSDaemon: Content must be initialized first.");
      return;
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
      NsdServiceInfo serviceInfo = new NsdServiceInfo();
      serviceInfo.setServiceName("o");
      serviceInfo.setServiceType("_x._tcp");
      serviceInfo.setPort(1);
      mRegistrationListener = new NsdManager.RegistrationListener() {
        @Override
        public void onServiceRegistered(NsdServiceInfo NsdServiceInfo) {}

        @Override
        public void onRegistrationFailed(NsdServiceInfo serviceInfo,
                                         int errorCode) {}

        @Override
        public void onServiceUnregistered(NsdServiceInfo arg0) {}

        @Override
        public void onUnregistrationFailed(NsdServiceInfo serviceInfo,
                                           int errorCode) {}
      };
      NsdManager nsdManager =
          (NsdManager) mContext.getSystemService(mContext.NSD_SERVICE);
      if (nsdManager != null) {
        nsdManager.registerService(serviceInfo, NsdManager.PROTOCOL_DNS_SD,
            mRegistrationListener);
      }
    }
  }

  /**
   * Releases the mdnsd daemon process
   */
  static void releaseMDNSDaemon() {
    if (null == mRegistrationListener) {
      Log.i(TAG, "acquireMDNSDaemon: MDNS Daemon is not running.");
      return;
    }

    if (null == mContext) {
      Log.e(TAG, "acquireMDNSDaemon: Content must be initialized first.");
      return;
    }

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
      NsdManager nsdManager =
          (NsdManager) mContext.getSystemService(mContext.NSD_SERVICE);
      if (nsdManager != null) {
        nsdManager.unregisterService(mRegistrationListener);
      }
      mRegistrationListener = null;
    }
  }
}
