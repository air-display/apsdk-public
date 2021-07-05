package com.sheensoftlab.sheenscreen_mb;

import android.app.Activity;
import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.util.Log;

import javax.net.ssl.HttpsURLConnection;

public class SheenScreenMBApplication extends Application implements Application.ActivityLifecycleCallbacks {
    private static final String TAG = "SheenScreenMBApp";
    private boolean isBackground = true;

    @Override
    public void onCreate() {
        super.onCreate();
        listenForScreenOff();
        listenForForeground();
        disableHostnameVerification();
    }

    @Override
    public void onTerminate() {
        super.onTerminate();

    }

    @Override
    public void onTrimMemory(int level) {
        super.onTrimMemory(level);
        if (level == TRIM_MEMORY_UI_HIDDEN) {
            isBackground = true;
            notifyBackground();
        }

    }

    @Override
    public void onActivityCreated(Activity activity, Bundle savedInstanceState) {

    }

    @Override
    public void onActivityStarted(Activity activity) {

    }

    @Override
    public void onActivityResumed(Activity activity) {
        if (isBackground) {
            isBackground = false;
            notifyForeground();
        }
    }

    @Override
    public void onActivityPaused(Activity activity) {

    }

    @Override
    public void onActivityStopped(Activity activity) {

    }

    @Override
    public void onActivitySaveInstanceState(Activity activity, Bundle outState) {

    }

    @Override
    public void onActivityDestroyed(Activity activity) {

    }

    private void listenForForeground() {
        registerActivityLifecycleCallbacks(this);
    }

    private void listenForScreenOff() {
        IntentFilter screenStateFilter = new IntentFilter(Intent.ACTION_SCREEN_OFF);
        registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                isBackground = true;
                notifyBackground();
            }
        }, screenStateFilter);
    }

    private void notifyForeground() {
        Log.d(TAG, "notifyForeground: ");
    }

    private void notifyBackground() {
        Log.d(TAG, "notifyBackground: ");
    }

    protected boolean isBackground() {
        return isBackground;
    }

    private void disableHostnameVerification() {
        HttpsURLConnection.setDefaultHostnameVerifier((s, sslSession) -> true);
    }
}
