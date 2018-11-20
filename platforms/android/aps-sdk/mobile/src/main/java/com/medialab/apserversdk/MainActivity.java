package com.medialab.apserversdk;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.dueeeke.videocontroller.StandardVideoController;
import com.dueeeke.videoplayer.player.IjkVideoView;
import com.dueeeke.videoplayer.player.PlayerConfig;

import com.medialab.airplay.AirPlayConfig;
import com.medialab.airplay.AirPlayHandler;
import com.medialab.airplay.AirPlayServer;
import com.medialab.airplay.PlaybackInfo;

import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";

    private String videoSource = "http://183.60.23.26/vhot2hls.tc.qq.com/AzEES84h6AwFBgTWaCAQHXMt-mDV0LF8Z_7H1BuK70Hs/mp4/0/NGMDIiK9f82wz1Qnm6dwVRszHmag_FJv-5Jpqj5BWqdRR1f1rtNDcg/X3jWcGMkTcIQW3bIm7wUQKL_cPT3d5ioDZk3BEjIQ-ns_e6QjuloyWmwTeHPxVc8U60LSW3hHsb3SpQS8LlNjbQuG0ra3XYz2MVUlf5ZpuFWhi9iedZB5cf9JDlFboS99Qf2SyOVL4_Bf00_gFqWMs3fRIWov3KK/w079053moiw.mp4/w079053moiw.mp4.av.m3u8?fn=mp4&bw=64&st=0&et=0&iv=&ivfn=&ivfc=&ivt=&ivs=&ivd=&ivl=&ftype=mp4&fbw=45&type=m3u8&drm=0&sdtfrom=v3000&platform=10403&appver=6.4.0.21959";

    private AirPlayServer airPlayServer = null;

    private boolean isServerStarted = false;

    private IjkVideoView videoPlayer = null;

    public void startVideo(String source) {
        videoSource = source;
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                videoPlayer.setUrl(videoSource);
                videoPlayer.start();
            }
        });
    }

    public void stopVideo() {
        this.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                videoPlayer.stopPlayback();
            }
        });
    }

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

        videoPlayer = findViewById(R.id.player_view);
        videoPlayer.setTitle("Tencent WeCast Player");
        videoPlayer.setVideoController(new StandardVideoController(this));
        videoPlayer.setUrl(videoSource); //设置视频地址

        PlayerConfig playerConfig = new PlayerConfig.Builder()
                //启用边播边缓存功能
                //.enableCache()
                //启用重力感应自动进入/退出全屏功能
                .autoRotate()
                //启动硬解码，启用后可能导致视频黑屏，音画不同步
                .enableMediaCodec()
                //启用SurfaceView显示视频，不调用默认使用TextureView
                .usingSurfaceView()
                //保存播放进度
                .savingProgress()
                //关闭AudioFocusChange监听
                .disableAudioFocus()
                //循环播放当前正在播放的视频
                .setLooping()
                .build();
        videoPlayer.setPlayerConfig(playerConfig);

        airPlayServer = new AirPlayServer(getApplicationContext());
        airPlayServer.setConfig(AirPlayConfig.defaultInstance());

        airPlayServer.setHandler(new AirPlayHandler(this) {
            private float pos = 0;

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
                Log.i(TAG, String.format("on_audio_set_volume: ratio = %f, volume = %f", ratio, volume));
            }

            @Override
            public void on_audio_set_progress(float ratio, long start, long current, long end) {
                Log.i(TAG, String.format("on_audio_set_progress: ratio = %f, start = %d, current = %d, end = %d",
                        ratio, start, current, end));
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
                Log.i(TAG, String.format("on_video_play: location = %s, start_pos = %f", location, start_pos));
                MainActivity activity = (MainActivity) getContext();
                activity.startVideo(location);
                pos = start_pos;
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
                MainActivity activity = (MainActivity) getContext();
                activity.stopVideo();
            }

            @Override
            public PlaybackInfo get_playback_info() {
                PlaybackInfo playbackInfo = new PlaybackInfo();
                playbackInfo.duration = 281;
                playbackInfo.position = ++pos;
                playbackInfo.rate = 1;
                Log.i(TAG, String.format("get_playback_info: duration = %f, position = %f", playbackInfo.duration, playbackInfo.position));
                return playbackInfo;
            }
        });
    }
}
