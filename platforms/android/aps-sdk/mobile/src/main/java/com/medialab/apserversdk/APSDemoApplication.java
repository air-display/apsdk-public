package com.medialab.apserversdk;

import android.app.Application;
import android.content.SharedPreferences;
import android.preference.Preference;
import android.preference.PreferenceManager;
import android.util.Log;

import com.google.android.exoplayer2.offline.DownloadManager;
import com.google.android.exoplayer2.offline.DownloaderConstructorHelper;
import com.google.android.exoplayer2.upstream.DataSource;
import com.google.android.exoplayer2.upstream.DefaultDataSourceFactory;
import com.google.android.exoplayer2.upstream.DefaultHttpDataSourceFactory;
import com.google.android.exoplayer2.upstream.FileDataSourceFactory;
import com.google.android.exoplayer2.upstream.HttpDataSource;
import com.google.android.exoplayer2.upstream.cache.Cache;
import com.google.android.exoplayer2.upstream.cache.CacheDataSource;
import com.google.android.exoplayer2.upstream.cache.CacheDataSourceFactory;
import com.google.android.exoplayer2.upstream.cache.NoOpCacheEvictor;
import com.google.android.exoplayer2.upstream.cache.SimpleCache;
import com.google.android.exoplayer2.util.Util;
import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayHandler;
import com.medialab.airplay.AirPlayServer;
import com.medialab.airplay.PlaybackInfo;

import java.io.File;

import javax.net.ssl.HttpsURLConnection;

public class APSDemoApplication extends Application {
    private static final String TAG = "APSDemoApplication";

    private static final String DOWNLOAD_ACTION_FILE = "actions";
    private static final String DOWNLOAD_TRACKER_ACTION_FILE = "tracked_actions";
    private static final String DOWNLOAD_CONTENT_DIRECTORY = "downloads";
    private static final int MAX_SIMULTANEOUS_DOWNLOADS = 2;

    private static final String DEVICE_UNIQUE_ID = "device_uid";

    protected String userAgent;

    private File downloadDirectory;
    private Cache downloadCache;
    private DownloadManager downloadManager;
    private APSDownloadTracker downloadTracker;

    private AirPlayServer airplayServer;

    public interface IAirPlayAcceptor {
        void preparePlayer(String location, float position);
    }
    private IAirPlayAcceptor airplayAcceptor;

    public interface IPlayerClient {
        void stop();
        void setScrub(float position);
        void setRate(float rate);
        PlaybackInfo getPlaybackInfo();
    }
    private IPlayerClient playerClient;

    @Override
    public void onCreate() {
        super.onCreate();
        userAgent = Util.getUserAgent(this, "APSDemo");
        disableHostnameVefification();
        createAirPlayServer();
        startAirPlayServer();
    }

    @Override
    public void onTerminate() {
        super.onTerminate();
        stopAirPlayServer();
    }

    /** Returns a {@link DataSource.Factory}. */
    public DataSource.Factory buildDataSourceFactory() {
        DefaultDataSourceFactory upstreamFactory =
                new DefaultDataSourceFactory(this, buildHttpDataSourceFactory());
        return buildReadOnlyCacheDataSource(upstreamFactory, getDownloadCache());
    }

    /** Returns a {@link HttpDataSource.Factory}. */
    public HttpDataSource.Factory buildHttpDataSourceFactory() {
        return new DefaultHttpDataSourceFactory(userAgent);
    }

    public void setAirplayAcceptor(IAirPlayAcceptor airplayAcceptor) {
        this.airplayAcceptor = airplayAcceptor;
    }

    public void setPlayerClient(IPlayerClient playerClient) {
        this.playerClient = playerClient;
    }

    public void startAirPlayServer() {
        if (null != airplayServer) {
            airplayServer.start();
            short port = airplayServer.getServicePort();
            Log.i(TAG, String.format("startAirPlayServer: AirPlay service started on port %d", port));
        }
    }

    public void stopAirPlayServer() {
        if (null != airplayServer) {
            airplayServer.stop();
        }
    }

    /** Returns whether extension renderers should be used. */
    public boolean useExtensionRenderers() {
        return "withExtensions".equals(BuildConfig.FLAVOR);
    }

    public DownloadManager getDownloadManager() {
        initDownloadManager();
        return downloadManager;
    }

    public APSDownloadTracker getDownloadTracker() {
        initDownloadManager();
        return downloadTracker;
    }

    private synchronized void initDownloadManager() {
        if (downloadManager == null) {
            DownloaderConstructorHelper downloaderConstructorHelper =
                    new DownloaderConstructorHelper(getDownloadCache(), buildHttpDataSourceFactory());
            downloadManager =
                    new DownloadManager(
                            downloaderConstructorHelper,
                            MAX_SIMULTANEOUS_DOWNLOADS,
                            DownloadManager.DEFAULT_MIN_RETRY_COUNT,
                            new File(getDownloadDirectory(), DOWNLOAD_ACTION_FILE));
            downloadTracker =
                    new APSDownloadTracker(
                            /* context= */ this,
                            buildDataSourceFactory(),
                            new File(getDownloadDirectory(), DOWNLOAD_TRACKER_ACTION_FILE));
            downloadManager.addListener(downloadTracker);
        }
    }

    private synchronized Cache getDownloadCache() {
        if (downloadCache == null) {
            File downloadContentDirectory = new File(getDownloadDirectory(), DOWNLOAD_CONTENT_DIRECTORY);
            downloadCache = new SimpleCache(downloadContentDirectory, new NoOpCacheEvictor());
        }
        return downloadCache;
    }

    private File getDownloadDirectory() {
        if (downloadDirectory == null) {
            downloadDirectory = getExternalFilesDir(null);
            if (downloadDirectory == null) {
                downloadDirectory = getFilesDir();
            }
        }
        return downloadDirectory;
    }

    private static CacheDataSourceFactory buildReadOnlyCacheDataSource(
            DefaultDataSourceFactory upstreamFactory, Cache cache) {
        return new CacheDataSourceFactory(
                cache,
                upstreamFactory,
                new FileDataSourceFactory(),
                /* cacheWriteDataSinkFactory= */ null,
                CacheDataSource.FLAG_IGNORE_CACHE_ON_ERROR,
                /* eventListener= */ null);
    }

    private void disableHostnameVefification() {
        HttpsURLConnection.setDefaultHostnameVerifier((s, sslSession) -> true);
    }

    private void createAirPlayServer() {
        if (airplayServer == null) {
            AirPlayConfig config = AirPlayConfig.defaultInstance();
            SharedPreferences preference = PreferenceManager.getDefaultSharedPreferences(this);
            if (preference.contains(DEVICE_UNIQUE_ID)) {
                config.setMacAddress(preference.getString(DEVICE_UNIQUE_ID, config.getMacAddress()));
            } else {
                preference
                        .edit()
                        .putString(DEVICE_UNIQUE_ID, config.getMacAddress())
                        .commit();

            }
            String name = String.format("APS[%s]", config.getDeviceID());
            config.setName(name);
            config.getDisplay().setWidth(1920);
            config.getDisplay().setHeight(1280);
            airplayServer = new AirPlayServer(getApplicationContext());
            airplayServer.setConfig(config);
        }

        airplayServer.setHandler(new AirPlayHandler(this) {
            private String videoSession;
            private PlaybackInfo lastPlaybackInfo;

            @Override
            public void on_mirror_stream_started() {
                Log.i(TAG, "on_mirror_stream_started: ");
            }

            @Override
            public void on_mirror_stream_codec(byte[] data) {
                Log.i(TAG, "on_mirror_stream_codec: ");
            }

            @Override
            public void on_mirror_stream_data(byte[] data, long timestamp) {
                Log.i(TAG, String.format("on_mirror_stream_data: length %d, timestamp %d", data.length, timestamp));
            }

            @Override
            public void on_mirror_stream_stopped() {
                Log.i(TAG, "on_mirror_stream_stopped: ");
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
            }

            @Override
            public void on_audio_stream_data(byte[] data) {
                Log.i(TAG, "on_audio_stream_data: ");
            }

            @Override
            public void on_audio_stream_stopped() {
                Log.i(TAG, "on_audio_stream_stopped: ");
            }

            @Override
            public void on_video_play(String session, String location, float position) {
                Log.i(TAG, String.format("on_video_play: location = %s, start_pos = %f", location, position));
                videoSession = session;
                if (null != airplayAcceptor) {
                    airplayAcceptor.preparePlayer(location, position);
                }
            }

            @Override
            public void on_video_scrub(String session, float position) {
                Log.i(TAG, String.format("on_video_scrub: position = %f", position));
                if (!session.equalsIgnoreCase(videoSession)) {
                    Log.e(TAG, "Invalid session id" + session);
                    return;
                }
                if (null != playerClient) {
                    playerClient.setScrub(position);
                }
            }

            @Override
            public void on_video_rate(String session, float value) {
                Log.i(TAG, String.format("on_video_rate: value = %f", value));
                if (!session.equalsIgnoreCase(videoSession)) {
                    Log.e(TAG, "Invalid session id" + session);
                    return;
                }
                if (null != playerClient) {
                    playerClient.setRate(value);
                }
            }

            @Override
            public void on_video_stop(String session) {
                Log.i(TAG, "on_video_stop: ");
                if (!session.equalsIgnoreCase(videoSession)) {
                    Log.e(TAG, "Invalid session id" + session);
                    return;
                }
                if (null != playerClient) {
                    playerClient.stop();
                }
            }

            @Override
            public PlaybackInfo get_playback_info(String session) {
                if (!session.equalsIgnoreCase(videoSession)) {
                    Log.e(TAG, "Invalid session id" + session);
                    return lastPlaybackInfo;
                }
                if (null != playerClient) {
                    lastPlaybackInfo = playerClient.getPlaybackInfo();
                    Log.i(TAG, String.format("get_playback_info: duration = %f, position = %f", lastPlaybackInfo.duration, lastPlaybackInfo.position));
                }
                return lastPlaybackInfo;
            }
        });
    }
}
