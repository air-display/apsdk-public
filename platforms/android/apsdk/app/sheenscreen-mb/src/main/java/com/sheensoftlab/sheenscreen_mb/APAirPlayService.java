package com.sheensoftlab.sheenscreen_mb;

import android.app.Service;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.IBinder;
import android.preference.PreferenceManager;
import android.util.Log;

import androidx.annotation.Nullable;

import com.sheensoftlab.apsdk.AirPlayConfig;
import com.sheensoftlab.apsdk.AirPlayServer;
import com.sheensoftlab.apsdk.AirPlaySession;
import com.sheensoftlab.apsdk.IAirPlayHandler;

import java.util.concurrent.ConcurrentHashMap;

public class APAirPlayService extends Service {
    private static final String TAG = "APAirPlayService";
    private static final String DEVICE_UNIQUE_ID = "device_uid";
    private static APAirPlayService mInstance = null;
    private AirPlayServer mAirplayServer;

    public static APAirPlayService getInstance() {
        return mInstance;
    }

    public APAirPlayService() {
    }

    // region session map
    private final ConcurrentHashMap<Long, AirPlaySession> sessionMap = new ConcurrentHashMap<>();

    public AirPlaySession lookupSession(long sid) {
        return sessionMap.get(sid);
    }

    protected void insertSession(AirPlaySession session) {
        sessionMap.put(session.getSessionId(), session);
    }

    protected void removeSession(long sid) {
        sessionMap.remove(sid);
    }
    // endregion

    private void createMirroringSession(AirPlaySession session) {
        Intent intent = new Intent(getApplicationContext(), APMirrorPlayerActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.putExtra(APMirrorPlayerActivity.AIRPLAY_SESSION_ID, session.getSessionId());
        startActivity(intent);

        synchronized (session) {
            try {
                session.wait();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
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

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate");
        mInstance = this;
        mAirplayServer = new AirPlayServer(getApplicationContext());

        AirPlayConfig config = AirPlayConfig.defaultInstance();
        config.setName(String.format("SheenScreen[%s]", config.getDeviceID()));
        config.setMacAddress(getDeviceUniqueId(config.getMacAddress()));
        config.getDisplay().setWidth(1920);
        config.getDisplay().setHeight(1280);
        mAirplayServer.setConfig(config);

        mAirplayServer.setHandler(new IAirPlayHandler() {
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
                Log.i(TAG, "---------------------- session end: " + session_id);
                AirPlaySession session = lookupSession(session_id);
                if (null != session) {
                    session.stopSession();
                }
                removeSession(session_id);
            }

            public void on_mirroring_session_begin(AirPlaySession session) {
                long sid = session.getSessionId();
                Log.i(TAG, "++++++++++++++++++++++ mirror session begins: " + sid);
                createMirroringSession(session);
            }

            public void on_casting_session_begin(AirPlaySession session) {
                long sid = session.getSessionId();
                Log.i(TAG, "++++++++++++++++++++++ cast session begins: " + sid);
                createCastingSession(session);
            }
        });
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "onStartCommand");
        if (null != mAirplayServer) {
            mAirplayServer.start();
        }
        return START_STICKY;
    }

    @Override
    public void onDestroy() {
        Log.i(TAG, "onDestroy");

        if (null != mAirplayServer) {
            mAirplayServer.stop();
        }
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
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
}