package com.medialab.apserversdk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayHandler;
import com.medialab.airplay.AirPlayServer;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";

    AirPlayServer airPlayServer = null;

    boolean isServerStarted = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final Button switch_btn = findViewById(R.id.server_switch);
        switch_btn.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Button btn = (Button)v;
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
        airPlayServer.setHandler(new AirPlayHandler() {
            @Override
            public void on_mirror_stream_started() {
                Log.i(TAG, "on_mirror_stream_started: ");
            }

            @Override
            public void on_mirror_stream_data(ByteBuffer data) {
                Log.i(TAG, "on_mirror_stream_data: ");
            }

            @Override
            public void on_mirror_stream_stopped() {
                Log.i(TAG, "on_mirror_stream_stopped: ");
            }

            @Override
            public void on_audio_set_volume(float ratio, float volume) {
                Log.i(TAG, "on_audio_set_volume: ");
            }

            @Override
            public void on_audio_set_progress(float ratio, long start, long current, long end) {
                Log.i(TAG, "on_audio_set_progress: ");
            }

            @Override
            public void on_audio_set_cover(String format, ByteBuffer data, long length) {
                Log.i(TAG, "on_audio_set_cover: ");
            }

            @Override
            public void on_audio_set_meta_data(ByteBuffer data, long length) {
                Log.i(TAG, "on_audio_set_meta_data: ");
            }

            @Override
            public void on_audio_stream_started() {
                Log.i(TAG, "on_audio_stream_started: ");
            }

            @Override
            public void on_audio_stream_data(ByteBuffer data) {
                Log.i(TAG, "on_audio_stream_data: ");
            }

            @Override
            public void on_audio_stream_stopped() {
                Log.i(TAG, "on_audio_stream_stopped: ");
            }

            @Override
            public void on_video_play(String location, float start_pos) {
                Log.i(TAG, "on_video_play: ");
            }

            @Override
            public void on_video_scrub(float position) {
                Log.i(TAG, "on_video_scrub: ");
            }

            @Override
            public void on_video_rate(float value) {
                Log.i(TAG, "on_video_rate: ");
            }

            @Override
            public void on_video_stop() {
                Log.i(TAG, "on_video_stop: ");
            }
        });
    }
}
