package com.sheensoftlab.sheenscreen_mb;

import android.app.Activity;
import android.app.Application;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.util.Log;

import com.sheensoftlab.apsdk.AirPlayConfig;
import com.sheensoftlab.apsdk.AirPlayServer;
import com.sheensoftlab.apsdk.AirPlaySession;
import com.sheensoftlab.apsdk.IAirPlayHandler;
import com.sheensoftlab.apsdk.IAirPlayMirroringHandler;

import java.io.IOException;
import java.io.PipedOutputStream;
import java.util.concurrent.ConcurrentHashMap;

import javax.net.ssl.HttpsURLConnection;

public class SheenScreenMBApplication extends Application implements Application.ActivityLifecycleCallbacks {
    private static final String TAG = "SheenScreenMBApp";
    private static final String DEVICE_UNIQUE_ID = "device_uid";
    private static SheenScreenMBApplication instance;
    private final ConcurrentHashMap<Long, AirPlaySession> sessionMap = new ConcurrentHashMap<>();
    private boolean isBackground = true;
    private AirPlayServer airplayServer;
    private IAirPlayHandler airplayHandler;

    public static SheenScreenMBApplication getInstance() {
        return instance;
    }

    public AirPlaySession getSession(Long sid) {
        return sessionMap.get(sid);
    }

    protected void insertSession(AirPlaySession session) {
        sessionMap.put(session.getSessionId(), session);
    }

    protected void removeSession(long sid) {
        sessionMap.remove(sid);
    }

    @Override
    public void onCreate() {
        super.onCreate();
        instance = this;
        createAirPlayHandler();
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
        initializeAirPlayServer();
    }

    private void notifyBackground() {
        Log.d(TAG, "notifyBackground: ");
        releaseAirPlayServer();
    }

    protected boolean isBackground() {
        return isBackground;
    }

    protected String getDeviceUniqueId(String defaultId) {
        String uid = defaultId;
        SharedPreferences preference = PreferenceManager.getDefaultSharedPreferences(this);
        if (preference.contains(DEVICE_UNIQUE_ID)) {
            uid = preference.getString(DEVICE_UNIQUE_ID, uid);
        } else {
            preference
                    .edit()
                    .putString(DEVICE_UNIQUE_ID, uid)
                    .apply();
        }
        return uid;
    }

    protected void initializeAirPlayServer() {
        if (null == airplayServer) {
            AirPlayConfig config = AirPlayConfig.defaultInstance();
            config.setName(String.format("iScreen[%s]", config.getDeviceID()));
            config.setMacAddress(getDeviceUniqueId(config.getMacAddress()));
            //config.setPublishService(false);
            config.getDisplay().setWidth(1920);
            config.getDisplay().setHeight(1280);

            airplayServer = new AirPlayServer(getApplicationContext());
            airplayServer.setConfig(config);
        }

        airplayServer.setHandler(airplayHandler);
        airplayServer.start();

        Log.i(TAG, String.format("startAirPlayServer: AirPlay service started on port %d", airplayServer.getServicePort()));
    }

    public void releaseAirPlayServer() {
        if (null != airplayServer) {
            airplayServer.stop();
            airplayServer = null;
        }
    }

    private void disableHostnameVerification() {
        HttpsURLConnection.setDefaultHostnameVerifier((s, sslSession) -> true);
    }

    private void createAirPlayHandler() {
        if (airplayHandler == null) {
            airplayHandler = new IAirPlayHandler() {
                @Override
                public void on_session_begin(AirPlaySession session) {
                    insertSession(session);
                    if (AirPlaySession.MIRROR_SESSION == session.getSessionType()) {
                        on_mirroring_session_begin(session);
                    } else if (AirPlaySession.VIDEO_SESSION == session.getSessionType()) {
                        on_casting_session_begin(session);
                    }
                }

                @Override
                public void on_session_end(long session_id) {
                    Log.i(TAG, "----------------------session end: " + session_id);
                    removeSession(session_id);
                }

                public void on_mirroring_session_begin(AirPlaySession session) {
                    long sid = session.getSessionId();
                    Log.i(TAG, "++++++++++++++++++++++mirror stream begins: " + sid);
                    createMirroringSession(session);
                }

                public void on_casting_session_begin(AirPlaySession session) {
                    long sid = session.getSessionId();
                    Log.i(TAG, "#######################video stream begins: " + sid);
                    createCastingSession(session);
                }
            };
        }
    }

    private void createMirroringSession(AirPlaySession session) {
        Intent intent = new Intent(getApplicationContext(), APMirrorPlayerActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra(APMirrorPlayerActivity.AIRPLAY_SESSION_ID, session.getSessionId());
        startActivity(intent);


        IAirPlayMirroringHandler h = new IAirPlayMirroringHandler() {
            private PipedOutputStream videoOutputStream;
            private PipedOutputStream audioOutputStream;

            @Override
            public void on_video_stream_started() {
                Log.d(TAG, "on_video_stream_started: ");
                videoOutputStream = new PipedOutputStream();

            }

            @Override
            public void on_video_stream_codec(byte[] data) {
                Log.i(TAG, "on_video_stream_codec: ");


            }

            @Override
            public void on_video_stream_data(byte[] data, long timestamp) {
                int frame_size = ((int) data[0]) << 24;
                frame_size += ((int) data[1]) << 16;
                frame_size += ((int) data[2]) << 8;
                frame_size += (int) data[3];
                //Log.v(TAG, "on_video_stream_data: length " + data.length + ", frame size: " + frame_size + ", timestamp " + timestamp);
                Log.v(TAG, String.format("P========:%02x, %02x, %02x, %02x ========= frame size %d",
                        data[0],
                        data[1],
                        data[2],
                        data[3],
                        frame_size
                ));
            }

            @Override
            public void on_video_stream_heartbeat() {
                Log.i(TAG, "on_video_stream_heartbeat: ");

            }

            @Override
            public void on_video_stream_stopped() {
                Log.i(TAG, "on_video_stream_stopped: ");
                try {
                    videoOutputStream.close();
                    videoOutputStream = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void on_audio_set_volume(float ratio, float volume) {
                Log.i(TAG, String.format("on_audio_set_volume: ratio = %f, volume = %f", ratio, volume));
            }

            @Override
            public void on_audio_set_progress(float ratio, long start, long current, long end) {
                Log.i(TAG, String.format("on_audio_set_progress: ratio = %f, start = %d, current = %d, end = %d",
                        ratio, start, current, end));
            }

            @Override
            public void on_audio_set_cover(String format, byte[] data) {
                Log.i(TAG, String.format("on_audio_set_cover: format %s, length %d", format, data.length));
            }

            @Override
            public void on_audio_set_meta_data(byte[] data) {
                Log.i(TAG, String.format("on_audio_set_meta_data: length %d", data.length));
            }

            @Override
            public void on_audio_stream_started(int format) {
                if (format == PCM) {
                    Log.i(TAG, "on_audio_stream_started: PCM");

                } else if (format == ALAC) {
                    Log.i(TAG, "on_audio_stream_started: ALAC");

                } else if (format == AAC) {
                    Log.i(TAG, "on_audio_stream_started: AAC");

                } else if (format == AACELD) {
                    Log.i(TAG, "on_audio_stream_started: AACELD");

                } else {
                    Log.i(TAG, "on_audio_stream_started: Unknown");
                }
                audioOutputStream = new PipedOutputStream();
            }

            @Override
            public void on_audio_stream_data(byte[] data, long timestamp) {
                Log.v(TAG, "on_audio_stream_data: " + data.length + ", timestamp: " + timestamp);
            }

            @Override
            public void on_audio_stream_stopped() {
                Log.i(TAG, "on_audio_stream_stopped: ");
                try {
                    audioOutputStream.close();
                    audioOutputStream = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        };
        session.setMirrorHandler(h);
    }

    private void createCastingSession(AirPlaySession session) {
        Intent intent = new Intent(getApplicationContext(), APCastPlayerActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra(APCastPlayerActivity.AIRPLAY_SESSION_ID, session.getSessionId());
        startActivity(intent);

        synchronized (session) {
            try {
                session.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}
