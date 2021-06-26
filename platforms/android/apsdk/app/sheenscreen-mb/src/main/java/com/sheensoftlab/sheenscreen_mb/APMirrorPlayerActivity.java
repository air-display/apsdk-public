package com.sheensoftlab.sheenscreen_mb;

import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.sheensoftlab.apsdk.AirPlaySession;
import com.sheensoftlab.apsdk.IAirPlayMirroringHandler;
import com.sheensoftlab.sheenscreen_mb.databinding.ActivityApmirrorPlayerBinding;

import org.junit.Assert;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class APMirrorPlayerActivity extends AppCompatActivity implements SurfaceHolder.Callback, IAirPlayMirroringHandler {
    public static final String AIRPLAY_SESSION_ID = "airplay_session_id";
    private static final String TAG = "APMirrorPlayerActivity";

    private ActivityApmirrorPlayerBinding binding;
    private long sessionId;
    private AirPlaySession session;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityApmirrorPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        binding.surfaceView.getHolder().addCallback(this);

        sessionId = getIntent().getLongExtra(AIRPLAY_SESSION_ID, 0);
        session = SheenScreenMBApplication.getInstance().getSession(sessionId);
        session.setMirrorHandler(this);

        synchronized (session) {
            session.notifyAll();
        }
    }

    @Override
    protected void onPostCreate(Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
    }

    // region Methods implementation of SufaceHolder.Callback
    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        //reconfigVideoDecoder();
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }
    // endregion

    // region Methods implementation of IAirPlayMirroringHandler
    @Override
    public void on_video_stream_started() {
        Log.d(TAG, "on_video_stream_started: ");
        openVideo();
    }

    @Override
    public void on_video_stream_codec(byte[] data) {
        Log.i(TAG, "on_video_stream_codec: ");
        configVideo(data);
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
        feedVideo(data);
    }

    @Override
    public void on_video_stream_heartbeat() {
        Log.i(TAG, "on_video_stream_heartbeat: ");

    }

    @Override
    public void on_video_stream_stopped() {
        Log.i(TAG, "on_video_stream_stopped: ");
        closeVideo();
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
    // endregion

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
        String codecMime = "";
        MediaFormat inputFormat = MediaFormat.createAudioFormat("", AP_AUDIO_SAMPLE_RATE, AP_AUDIO_CHANNEL_COUNT);
        if (format == ALAC) {
            Log.i(TAG, "on_audio_stream_started: ALAC");
            codecMime = "audio/alac";
            inputFormat.setString(MediaFormat.KEY_MIME, codecMime);
        } else if (format == AAC) {
            Log.i(TAG, "on_audio_stream_started: AAC");
            codecMime = "audio/mp4a-latm";
            inputFormat.setString(MediaFormat.KEY_MIME, codecMime);
        } else if (format == AACELD) {
            Log.i(TAG, "on_audio_stream_started: AACELD");
            codecMime = "audio/mp4a-latm";
            inputFormat.setString(MediaFormat.KEY_MIME, codecMime);
            int AP_AUDIO_AAC_PROFILE = MediaCodecInfo.CodecProfileLevel.AACObjectELD;
            inputFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, AP_AUDIO_AAC_PROFILE);
            byte[] AP_AUDIO_AAC_RAW_CONFIG = new byte[]{(byte) 0xF8, (byte) 0xE8, (byte) 0x50, (byte) 0x00};
            inputFormat.setByteBuffer("csd-0", ByteBuffer.wrap(AP_AUDIO_AAC_RAW_CONFIG));
        } else if (format == PCM) {
            Log.i(TAG, "on_audio_stream_started: PCM");
        } else {
            Log.i(TAG, "on_audio_stream_started: Unknown");
        }

        openAudio(codecMime, inputFormat);
    }

    @Override
    public void on_audio_stream_data(byte[] data, long timestamp) {
        Log.v(TAG, "on_audio_stream_data: " + data.length + ", timestamp: " + timestamp);
        feedAudio(data);
    }

    @Override
    public void on_audio_stream_stopped() {
        Log.i(TAG, "on_audio_stream_stopped: ");
        closeAudio();
    }

    // region Audio Player
    private MediaCodec audioDecoder;
    private AudioTrack audioTrack;
    private MediaCodec.BufferInfo audioOutputBufferInfo;
    private final int AP_AUDIO_SAMPLE_RATE = 44100;
    private final int AP_AUDIO_CHANNEL_COUNT = 2;
    private final int AP_AUDIO_PCM_ENCODING = AudioFormat.ENCODING_PCM_16BIT;
    private final int AP_AUDIO_CHANNEL_MASK = AudioFormat.CHANNEL_OUT_STEREO;

    private void openAudio(String codecMime, MediaFormat inputFormat) {
        audioOutputBufferInfo = new MediaCodec.BufferInfo();
        // create and config the decoder
        try {
            audioDecoder = MediaCodec.createDecoderByType(codecMime);
            if (null == audioDecoder) {
                Log.e(TAG, "openAudio: Failed to ceate the audio decoder");
                return;
            }
//            audioDecoder.setCallback(new MediaCodec.Callback() {
//                @Override
//                public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
//
//                }
//
//                @Override
//                public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//
//                }
//
//                @Override
//                public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
//
//                }
//
//                @Override
//                public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
//
//                }
//            });
            audioDecoder.configure(inputFormat, null, null, 0);
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        // create the audio renderer
        reconfigAudioRederer();

        // start the decoder
        audioDecoder.start();
    }

    private void feedAudio(byte[] data) {
        if (null == audioDecoder || null == audioTrack) {
            return;
        }

        processAudio(data);
    }

    private void closeAudio() {
        if (null != audioDecoder) {
            audioDecoder.stop();
            audioDecoder.release();
            audioDecoder = null;
        }

        if (audioTrack != null) {
            audioTrack.stop();
            audioTrack = null;
        }
    }

    void reconfigAudioRederer() {
        if (null != audioTrack) {
            audioTrack.stop();
            audioTrack = null;
        }

        MediaFormat outputFormat = audioDecoder.getOutputFormat();
        int outSmapleRate = AP_AUDIO_SAMPLE_RATE;
        int outChannelMask = AP_AUDIO_CHANNEL_MASK;
        int outPCMEncoding = AP_AUDIO_PCM_ENCODING;
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.Q) {
            outSmapleRate = outputFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE, AP_AUDIO_SAMPLE_RATE);
            outChannelMask = outputFormat.getInteger(MediaFormat.KEY_CHANNEL_MASK, AP_AUDIO_CHANNEL_MASK);
            outPCMEncoding = outputFormat.getInteger(MediaFormat.KEY_PCM_ENCODING, AP_AUDIO_PCM_ENCODING);
        }
        int bufferSizeInBytes = AudioTrack.getMinBufferSize(outSmapleRate, outChannelMask, outPCMEncoding);
        audioTrack = new AudioTrack(
                new AudioAttributes.Builder()
                        .setUsage(AudioAttributes.USAGE_MEDIA)
                        .setContentType(AudioAttributes.CONTENT_TYPE_MUSIC)
                        .build(),
                new AudioFormat.Builder()
                        .setSampleRate(outSmapleRate)
                        .setChannelMask(outChannelMask)
                        .setEncoding(outPCMEncoding)
                        .build(),
                bufferSizeInBytes,
                AudioTrack.MODE_STREAM,
                AudioManager.AUDIO_SESSION_ID_GENERATE);
        audioTrack.play();
    }

    void processAudio(byte[] data) {
        int inIndex = audioDecoder.dequeueInputBuffer(0);
        if (inIndex >= 0) {
            audioDecoder.getInputBuffer(inIndex).put(data);
            audioDecoder.queueInputBuffer(inIndex, 0, data.length, 0, 0);
        }

        int outIndex = audioDecoder.dequeueOutputBuffer(audioOutputBufferInfo, 0);
        switch (outIndex) {
            case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                break;
            case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                Log.d(TAG, "new format " + audioDecoder.getOutputFormat());
                reconfigAudioRederer();
                break;
            case MediaCodec.INFO_TRY_AGAIN_LATER:
                Log.d(TAG, "dequeueOutputBuffer timed out!");
                break;
            default:
                audioTrack.write(audioDecoder.getOutputBuffer(outIndex), audioOutputBufferInfo.size, AudioTrack.WRITE_BLOCKING);
                audioDecoder.releaseOutputBuffer(outIndex, false);
                break;
        }
    }
    // endregion

    // region Video Player
    MediaCodec videoDecoder;
    private MediaCodec.BufferInfo videoOutputBufferInfo;
    private AvcCConfig avcCConfig;

    private final String AP_VIDEO_FORMAT_MIME = "video/avc";

    private void openVideo() {
        videoOutputBufferInfo = new MediaCodec.BufferInfo();

        // create and config the decoder
        try {
            videoDecoder = MediaCodec.createDecoderByType(AP_VIDEO_FORMAT_MIME);
            if (null == videoDecoder) {
                Log.e(TAG, "openAudio: Failed to ceate the video decoder");
                return;
            }
//            videoDecoder.setCallback(new MediaCodec.Callback() {
//                @Override
//                public void onInputBufferAvailable(@NonNull MediaCodec mediaCodec, int i) {
//
//                }
//
//                @Override
//                public void onOutputBufferAvailable(@NonNull MediaCodec mediaCodec, int i, @NonNull MediaCodec.BufferInfo bufferInfo) {
//
//                }
//
//                @Override
//                public void onError(@NonNull MediaCodec mediaCodec, @NonNull MediaCodec.CodecException e) {
//
//                }
//
//                @Override
//                public void onOutputFormatChanged(@NonNull MediaCodec mediaCodec, @NonNull MediaFormat mediaFormat) {
//
//                }
//            });
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
    }

    private void configVideo(byte[] config) {
        avcCConfig = new AvcCConfig(config);
        innerConfigVideoDecoder(avcCConfig);
    }

    private void feedVideo(byte[] data) {
        if (null == videoDecoder) {
            return;
        }

        processVideo(data);
    }

    private void closeVideo() {
        if (null != videoDecoder) {
            videoDecoder.stop();
            videoDecoder.release();
            videoDecoder = null;
        }
     }

    private void innerConfigVideoDecoder(AvcCConfig avcCConfig) {
        if (null == videoDecoder) {
            return;
        }
        videoDecoder.reset();

        try {
            MediaFormat format = MediaFormat.createVideoFormat(AP_VIDEO_FORMAT_MIME, 1920, 1080);
            format.setByteBuffer("csd-0", avcCConfig.toAnnexB());
            videoDecoder.configure(format, binding.surfaceView.getHolder().getSurface(), null, 0);

        } catch (Exception e) {
            e.printStackTrace();
            return;
        }

        videoDecoder.start();
    }

    private void reconfigVideoDecoder() {
        innerConfigVideoDecoder(avcCConfig);
    }

    private void processVideo(byte[] data) {
        int inIndex = videoDecoder.dequeueInputBuffer(0);
        if (inIndex >= 0) {
            int nalLenSize = avcCConfig.nalLengthSize;
            if (data.length >= nalLenSize) {
                data[--nalLenSize] = 1;
                while(--nalLenSize >= 0) data[nalLenSize] = 0;
            }
            videoDecoder.getInputBuffer(inIndex).put(data);
            videoDecoder.queueInputBuffer(inIndex, 0, data.length, 0, 0);
        }

        int outIndex = videoDecoder.dequeueOutputBuffer(videoOutputBufferInfo, 0);
        switch (outIndex) {
            case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED:
                break;
            case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED:
                Log.d(TAG, "new format " + videoDecoder.getOutputFormat());
                break;
            case MediaCodec.INFO_TRY_AGAIN_LATER:
                Log.d(TAG, "dequeueOutputBuffer timed out!");
                break;
            default:
                videoDecoder.releaseOutputBuffer(outIndex, true);
                break;
        }
    }
    // endregion

    private class AvcCConfig {
        private int profile;
        private int profileCompat;
        private int level;
        private int nalLengthSize;

        private List<ByteBuffer> spsList;
        private List<ByteBuffer> ppsList;

        public AvcCConfig() {
            this.spsList = new ArrayList<ByteBuffer>();
            this.ppsList = new ArrayList<ByteBuffer>();
        }

        public AvcCConfig(byte[] data) {
            this.spsList = new ArrayList<ByteBuffer>();
            this.ppsList = new ArrayList<ByteBuffer>();
            parse(ByteBuffer.wrap(data));
        }

        public void parse(ByteBuffer input) {
            input.position(input.position() + Math.min(input.remaining(), 1));
            profile = input.get() & 0xff;
            profileCompat = input.get() & 0xff;
            level = input.get() & 0xff;
            int flags = input.get() & 0xff;
            nalLengthSize = (flags & 0x03) + 1;

            int nSPS = input.get() & 0x1f; // 3 bits reserved + 5 bits number of
            // sps
            for (int i = 0; i < nSPS; i++) {
                int spsSize = input.getShort();
                Assert.assertEquals(0x27, input.get() & 0x3f);

                ByteBuffer spsSlice = input.duplicate();
                int limit = input.position() + spsSize - 1;
                spsSlice.limit(limit);
                input.position(limit);
                spsList.add(spsSlice);
            }

            int nPPS = input.get() & 0xff;
            for (int i = 0; i < nPPS; i++) {
                int ppsSize = input.getShort();
                Assert.assertEquals(0x28, input.get() & 0x3f);

                ByteBuffer ppsSlice = input.duplicate();
                int limit = input.position() + ppsSize - 1;
                ppsSlice.limit(limit);
                input.position(limit);
                ppsList.add(ppsSlice);
            }
        }

        public void generate(ByteBuffer out) {
            out.put((byte) 0x1); // version
            out.put((byte) profile);
            out.put((byte) profileCompat);
            out.put((byte) level);
            out.put((byte) 0xff);

            out.put((byte) (spsList.size() | 0xe0));
            for (ByteBuffer sps : spsList) {
                out.putShort((short) (sps.remaining() + 1));
                out.put((byte) 0x67);

                if (sps.hasArray()) {
                    out.put(sps.array(), sps.arrayOffset() + sps.position(), Math.min(out.remaining(), sps.remaining()));
                } else {
                    out.put(sps.duplicate().get(new byte[Math.min(sps.remaining(), out.remaining())]));
                }
            }

            out.put((byte) ppsList.size());
            for (ByteBuffer pps : ppsList) {
                out.putShort((byte) (pps.remaining() + 1));
                out.put((byte) 0x68);

                if (pps.hasArray()) {
                    out.put(pps.array(), pps.arrayOffset() + pps.position(), Math.min(out.remaining(), pps.remaining()));
                } else {
                    out.put(pps.duplicate().get(new byte[Math.min(pps.remaining(), out.remaining())]));
                }
            }
        }

        public int getProfile() {
            return profile;
        }

        public int getProfileCompat() {
            return profileCompat;
        }

        public int getLevel() {
            return level;
        }

        public List<ByteBuffer> getSpsList() {
            return spsList;
        }

        public List<ByteBuffer> getPpsList() {
            return ppsList;
        }

        public int getNalLengthSize() {
            return nalLengthSize;
        }

        public ByteBuffer toAnnexB() {
            int totalCodecPrivateSize = 0;
            for (ByteBuffer byteBuffer : spsList) {
                totalCodecPrivateSize += byteBuffer.remaining() + 5;
            }
            for (ByteBuffer byteBuffer : ppsList) {
                totalCodecPrivateSize += byteBuffer.remaining() + 5;
            }

            ByteBuffer bb = ByteBuffer.allocate(totalCodecPrivateSize);
            for (ByteBuffer byteBuffer : spsList) {
                bb.putInt(1);
                bb.put((byte) 0x67);
                bb.put(byteBuffer.duplicate());
            }
            for (ByteBuffer byteBuffer : ppsList) {
                bb.putInt(1);
                bb.put((byte) 0x68);
                bb.put(byteBuffer.duplicate());
            }
            bb.flip();
            return bb;
        }
    }

}