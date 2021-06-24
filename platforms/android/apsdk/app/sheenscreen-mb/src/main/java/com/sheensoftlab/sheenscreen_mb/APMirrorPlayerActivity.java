package com.sheensoftlab.sheenscreen_mb;

import android.media.AudioTrack;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;

import com.sheensoftlab.apsdk.AirPlaySession;
import com.sheensoftlab.apsdk.IAirPlayMirroringHandler;
import com.sheensoftlab.sheenscreen_mb.databinding.ActivityApmirrorPlayerBinding;

import java.util.PrimitiveIterator;

public class APMirrorPlayerActivity extends AppCompatActivity implements IAirPlayMirroringHandler {
    public static final String AIRPLAY_SESSION_ID = "airplay_session_id";

    private ActivityApmirrorPlayerBinding binding;

    private long sessionId;

    private AirPlaySession session;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityApmirrorPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        sessionId = getIntent().getLongExtra(AIRPLAY_SESSION_ID, 0);
        session = SheenScreenMBApplication.getInstance().getSession(sessionId);
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
    }

    // region Methods implementation of IAirPlayMirroringHandler
    @Override
    public void on_video_stream_started() {

    }

    @Override
    public void on_video_stream_codec(byte[] data) {

    }

    @Override
    public void on_video_stream_data(byte[] data, long timestamp) {

    }

    @Override
    public void on_video_stream_heartbeat() {

    }

    @Override
    public void on_video_stream_stopped() {

    }

    @Override
    public void on_audio_set_volume(float ratio, float volume) {

    }

    @Override
    public void on_audio_set_progress(float ratio, long start, long current, long end) {

    }

    @Override
    public void on_audio_set_cover(String format, byte[] data) {

    }

    @Override
    public void on_audio_set_meta_data(byte[] data) {

    }

    @Override
    public void on_audio_stream_started(int format) {

    }

    @Override
    public void on_audio_stream_data(byte[] data, long timestamp) {

    }

    @Override
    public void on_audio_stream_stopped() {

    }
    // endregion

    // region Audio Player
    private AudioTrack audioTrack;

    private void openAudio() {

    }

    private void feedAudio() {

    }
    // endregion

    // region Video Player
    private  void openVideo() {

    }

    private void feedVideo() {

    }
    // endregion

}