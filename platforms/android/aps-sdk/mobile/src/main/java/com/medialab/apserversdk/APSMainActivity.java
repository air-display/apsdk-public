package com.medialab.apserversdk;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayHandler;
import com.medialab.airplay.AirPlayServer;
import com.medialab.airplay.MirroringVideoCodec;
import com.medialab.airplay.PlaybackInfo;

public class APSMainActivity extends AppCompatActivity {
    private static final String TAG = "APSMainActivity";

    private AirPlayServer airPlayServer = null;

    private boolean isServerStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button switch_btn = findViewById(R.id.server_switch);
        switch_btn.setOnClickListener(new View.OnClickListener() {
                                          public void onClick(View v) {
                                              Button btn = (Button) v;
                                              if (isServerStarted) {
                                                  airPlayServer.stop();
                                                  btn.setText("Start");
                                                  isServerStarted = false;
                                              } else {
                                                  isServerStarted = airPlayServer.start();
                                                  if (!isServerStarted) {
                                                      Toast.makeText(
                                                              getApplicationContext(),
                                                              "Failed to start the server!",
                                                              Toast.LENGTH_SHORT)
                                                              .show();
                                                  } else {
                                                      btn.setText("Stop");
                                                  }
                                              }
                                          }
                                      });

        airPlayServer = new AirPlayServer(getApplicationContext());
        airPlayServer.setConfig(AirPlayConfig.defaultInstance());

        airPlayServer.setHandler(new AirPlayHandler(this) {
            private float pos = 0;

            @Override
            public void on_mirror_stream_started() {
                Log.i(TAG, "on_mirror_stream_started: ");
            }

            @Override
            public void on_mirror_stream_codec(MirroringVideoCodec codec) {
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
            public void on_video_play(String location, float start_pos) {
                Log.i(TAG, String.format("on_video_play: location = %s, start_pos = %f", location, start_pos));
                Intent intent = new Intent(APSMainActivity.this, APSPlayerActivity.class);
                intent.putExtra(APSPlayerActivity.PREFER_EXTENSION_DECODERS_EXTRA, false);
                intent.putExtra(APSPlayerActivity.START_WINDOW_INDEX, 0);
                intent.putExtra(APSPlayerActivity.START_POSITION, start_pos);
                intent.setAction(APSPlayerActivity.ACTION_VIEW);
                intent.setData(Uri.parse(location));
                startActivity(intent);
            }

            @Override
            public void on_video_scrub(float position) {
                Log.i(TAG, String.format("on_video_scrub: position = %f", position));
            }

            @Override
            public void on_video_rate(float value) {
                Log.i(TAG, String.format("on_video_rate: value = %f", value));
            }

            @Override
            public void on_video_stop() {
                Log.i(TAG, "on_video_stop: ");
                APSMainActivity activity = (APSMainActivity) getContext();
            }

            @Override
            public PlaybackInfo get_playback_info() {
                PlaybackInfo playbackInfo = new PlaybackInfo();
                playbackInfo.duration = 281;
                pos += 1.0f;
                playbackInfo.position = pos;
                playbackInfo.rate = 1;
                playbackInfo.stallCount = 8976;
                Log.i(TAG, String.format("get_playback_info: duration = %f, position = %f", playbackInfo.duration, playbackInfo.position));
                return playbackInfo;
            }
        });
    }
}
