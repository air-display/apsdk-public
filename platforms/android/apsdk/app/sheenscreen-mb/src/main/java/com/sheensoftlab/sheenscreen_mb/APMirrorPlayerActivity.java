package com.sheensoftlab.sheenscreen_mb;

import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.media.AudioAttributes;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.os.Build;
import android.os.Bundle;
import android.os.ConditionVariable;
import android.util.Log;
import android.view.OrientationEventListener;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.collection.CircularArray;

import com.google.android.exoplayer2.ParserException;
import com.google.android.exoplayer2.util.ParsableByteArray;
import com.google.android.exoplayer2.video.AvcConfig;
import com.sheensoftlab.apsdk.AirPlaySession;
import com.sheensoftlab.apsdk.IAirPlayMirroringHandler;
import com.sheensoftlab.sheenscreen_mb.databinding.ActivityApmirrorPlayerBinding;

import java.nio.ByteBuffer;

import static java.lang.Thread.MAX_PRIORITY;

public class APMirrorPlayerActivity extends AppCompatActivity implements IAirPlayMirroringHandler, AirPlaySession.StopHandler {
    private static final String TAG = "APMirrorPlayerActivity";
    public static final String AIRPLAY_SESSION_ID = "airplay_session";

    private ActivityApmirrorPlayerBinding binding;
    private AirPlaySession session;
    private AudioPlayer audioPlayer;
    private VideoPlayer videoPlayer;

    // region SampleRingBuffer
    class SampleRingBuffer {
        private final CircularArray<byte[]> buffer;
        private final int capacity;

        public SampleRingBuffer(int capacity) {
            int DEFAULT_CAPACITY = 16;
            this.capacity = capacity > 0 ? capacity : DEFAULT_CAPACITY;
            this.buffer = new CircularArray<>(this.capacity);
        }

        public boolean tryPush(byte[] data) {
            synchronized (buffer) {
                if (buffer.size() >= capacity) {
                    return false;
                }

                buffer.addLast(data);
                return true;
            }
        }

        public void push(byte[] data) {
            synchronized (buffer) {
                if (buffer.size() >= capacity) {
                    buffer.popFirst();
                }

                buffer.addLast(data);
            }
        }

        public byte[] pop() {
            synchronized (buffer) {
                if (buffer.isEmpty()) {
                    return null;
                }

                return buffer.popFirst();
            }
        }

        public void clear() {
            synchronized (buffer) {
                buffer.clear();
            }
        }
    }
    // endregion

    // region AudioPlayer
    class AudioPlayer implements Runnable {
        private final String TAG = "AudioPlayer";
        private final SampleRingBuffer m_buffer;
        private final ConditionVariable m_signal;

        private Thread m_worker;
        private MediaCodec m_decoder;
        private AudioTrack m_renderer;

        public AudioPlayer() {
            m_buffer = new SampleRingBuffer(8);
            m_signal = new ConditionVariable();
        }

        public boolean initialize(String codecMime, MediaFormat inputFormat) {
            if (null != m_worker) {
                throw new RuntimeException("worker thread is running already.");
            }

            if (!configMediaCodec(codecMime, inputFormat)) {
                return false;
            }

            m_worker = new Thread(this);
            m_worker.setPriority(MAX_PRIORITY);

            return true;
        }

        public void start() throws RuntimeException {
            if (null == m_worker) {
                throw new RuntimeException("video worker hasn't been initialized.");
            }

            // start the worker
            m_worker.start();
        }

        public void setVolume(float vol) {
            if (null != m_renderer) {
                m_renderer.setVolume(vol);
            }
        }

        public void feedAudioData(byte[] data) {
            m_buffer.push(data);
        }

        public void stop() {
            if (null == m_worker) {
                return;
            }

            // stop worker thread
            m_signal.open();
            try {
                m_worker.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        public void uninitialize() throws RuntimeException {
            if (null != m_worker && Thread.State.TERMINATED != m_worker.getState()) {
                throw new RuntimeException("video worker is still running.");
            }

            if (null != m_worker) {
                m_worker = null;
            }

            // stop audio decoder and track
            if (null != m_decoder) {
                m_decoder.stop();
                m_decoder = null;
            }

            if (null != m_renderer) {
                m_renderer.stop();
                m_renderer = null;
            }
        }

        @Override
        public void run() {
            Log.w(TAG, "audio worker thread is starting");

            byte[] sample = null;
            int inputIndex, outputIndex;
            MediaCodec.BufferInfo outputBufferInfo = new MediaCodec.BufferInfo();

            m_signal.close();
            while (!m_signal.block(5)) {
                // get decoded sample
                outputIndex = m_decoder.dequeueOutputBuffer(outputBufferInfo, 0);
                switch (outputIndex) {
                    case MediaCodec.INFO_TRY_AGAIN_LATER: {
                        Log.v(TAG, "audio output buffer not avaiable");
                        break;
                    }
                    case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED: {
                        Log.d(TAG, "audio output format changed: " + m_decoder.getOutputFormat());
                        configAudioTrack();
                        break;
                    }
                    case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED: {
                        Log.d(TAG, "audio output buffers changed");
                        break;
                    }
                    default: {
                        if (null != m_renderer) {
                            m_renderer.write(m_decoder.getOutputBuffer(outputIndex), outputBufferInfo.size, AudioTrack.WRITE_BLOCKING);
                        }
                        m_decoder.releaseOutputBuffer(outputIndex, false);
                        break;
                    }
                }

                if (null == sample) {
                    // get data from ring buffer
                    sample = m_buffer.pop();
                }

                if (null == sample) {
                    // no data to process
                    continue;
                }

                // get input buffer
                inputIndex = m_decoder.dequeueInputBuffer(0);
                if (inputIndex < 0) {
                    Log.v(TAG, "audio input buffer not avaiable");
                    continue;
                }

                // fill the input buffer and return the buffer back
                m_decoder.getInputBuffer(inputIndex).put(sample);
                m_decoder.queueInputBuffer(inputIndex, 0, sample.length, 0, 0);

                // reset the data
                sample = null;
            }
            Log.w(TAG, "audio worker thread is exiting");
        }

        protected boolean configMediaCodec(String codecMime, MediaFormat inputFormat) {
            // create and config the decoder
            try {
                m_decoder = MediaCodec.createDecoderByType(codecMime);
                if (null == m_decoder) {
                    Log.e(TAG, "failed to ceate the audio decoder");
                    return false;
                }
                m_decoder.configure(inputFormat, null, null, 0);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }

            m_decoder.start();
            return true;
        }

        protected void configAudioTrack() {
            // build the media format
            MediaFormat outputFormat = m_decoder.getOutputFormat();
            int outSmapleRate = 44100;
            if (outputFormat.containsKey(MediaFormat.KEY_SAMPLE_RATE)) {
                outSmapleRate = outputFormat.getInteger(MediaFormat.KEY_SAMPLE_RATE);
            }

            int outChannelMask = 0;
            if (outputFormat.containsKey(MediaFormat.KEY_CHANNEL_MASK)) {
                outChannelMask = outputFormat.getInteger(MediaFormat.KEY_CHANNEL_MASK);
            }

            int outPCMEncoding = AudioFormat.ENCODING_PCM_16BIT;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                if (outputFormat.containsKey(MediaFormat.KEY_PCM_ENCODING)) {
                    outPCMEncoding = outputFormat.getInteger(MediaFormat.KEY_PCM_ENCODING);
                }
            }

            int bufferSizeInBytes = AudioTrack.getMinBufferSize(outSmapleRate, outChannelMask, outPCMEncoding);
            m_renderer = new AudioTrack(
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

            m_renderer.play();
        }
    }
    // endregion

    // region VideoPlayer
    class VideoPlayer implements Runnable, SurfaceHolder.Callback {
        private final String TAG = "VideoPlayer";
        private final SampleRingBuffer m_buffer;
        private final ConditionVariable m_signal;
        private final SurfaceView m_surfaceView;
        private final OrientationEventListener m_orientationEventListener;

        private Thread m_worker;
        private AvcConfig m_avcConfig;
        private MediaCodec m_decoder;

        public VideoPlayer(@NonNull SurfaceView surfaceView) {
            m_buffer = new SampleRingBuffer(16);
            m_signal = new ConditionVariable();
            m_surfaceView = surfaceView;

            m_orientationEventListener = new OrientationEventListener(m_surfaceView.getContext()) {
                @Override
                public void onOrientationChanged(int orientation) {
                    adjustVideoSurface();
                }
            };
            m_orientationEventListener.enable();
        }

        // region Methods implementation of SufaceHolder.Callback
        @Override
        public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {
        }

        @Override
        public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        }

        @Override
        public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {
        }
        // endregion

        public boolean initialize(String codecMime, byte[] rawConfig) {
            if (null != m_worker) {
                throw new RuntimeException("worker thread is running already.");
            }

            try {
                m_avcConfig = AvcConfig.parse(new ParsableByteArray(rawConfig));
            } catch (ParserException e) {
                e.printStackTrace();
                return false;
            }

            MediaFormat format = MediaFormat.createVideoFormat(codecMime, m_avcConfig.width, m_avcConfig.height);
            format.setByteBuffer("csd-0", buildCodecCsd());
            if (!configMediaCodec(codecMime, format)) {
                return false;
            }

            m_worker = new Thread(this);
            m_worker.setPriority(MAX_PRIORITY);

            return true;
        }

        public void start() throws RuntimeException {
            if (null == m_worker) {
                throw new RuntimeException("video worker hasn't been initialized.");
            }

            // start the worker
            m_worker.start();
        }

        public void feedVideoData(byte[] data) {
            convertAvccToAnnexB(data);
            m_buffer.push(data);
        }

        public void stop() {
            if (null == m_worker) {
                return;
            }

            // stop worker thread
            m_signal.open();
            try {
                m_worker.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        public void uninitialize() throws RuntimeException {
            if (null != m_worker && Thread.State.TERMINATED != m_worker.getState()) {
                throw new RuntimeException("video worker is still running.");
            }

            if (null != m_worker) {
                m_worker = null;
            }

            // stop audio decoder and track
            if (null != m_decoder) {
                m_decoder.stop();
                m_decoder = null;
            }
        }

        @Override
        public void run() {
            Log.w(TAG, "video worker thread is starting");

            byte[] sample = null;
            int inputIndex, outputIndex;
            MediaCodec.BufferInfo outputBufferInfo = new MediaCodec.BufferInfo();

            m_signal.close();
            while (!m_signal.block(10)) {
                // get decoded sample
                outputIndex = m_decoder.dequeueOutputBuffer(outputBufferInfo, 0);
                switch (outputIndex) {
                    case MediaCodec.INFO_TRY_AGAIN_LATER: {
                        Log.v(TAG, "video output buffer not avaiable");
                        break;
                    }
                    case MediaCodec.INFO_OUTPUT_FORMAT_CHANGED: {
                        adjustVideoSurface();
                        Log.d(TAG, "video output format changed: " + m_decoder.getOutputFormat());
                        break;
                    }
                    case MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED: {
                        Log.d(TAG, "video output buffers changed");
                        break;
                    }
                    default: {
                        m_decoder.releaseOutputBuffer(outputIndex, true);
                        break;
                    }
                }

                if (null == sample) {
                    // get data from ring buffer
                    sample = m_buffer.pop();
                }

                if (null == sample) {
                    // no data to process
                    continue;
                }

                // get input buffer
                inputIndex = m_decoder.dequeueInputBuffer(0);
                if (inputIndex < 0) {
                    Log.v(TAG, "video input buffer not avaiable");
                    continue;
                }

                // fill the input buffer and return the buffer back
                m_decoder.getInputBuffer(inputIndex).put(sample);
                m_decoder.queueInputBuffer(inputIndex, 0, sample.length, 0, 0);

                // reset the data
                sample = null;
            }
            Log.w(TAG, "video worker thread is exiting");
        }

        protected ByteBuffer buildCodecCsd() {
            int totalCodecPrivateSize = 0;
            for (byte[] data : m_avcConfig.initializationData) {
                totalCodecPrivateSize += data.length;
            }
            ByteBuffer bb = ByteBuffer.allocate(totalCodecPrivateSize);
            for (byte[] data : m_avcConfig.initializationData) {
                bb.put(data);
            }
            bb.flip();
            return bb;
        }

        protected boolean configMediaCodec(String codecMime, MediaFormat inputFormat) {
            // create and config the decoder
            try {
                m_decoder = MediaCodec.createDecoderByType(codecMime);
                if (null == m_decoder) {
                    Log.e(TAG, "failed to ceate the video decoder");
                    return false;
                }

                m_surfaceView.getHolder().addCallback(this);
                m_decoder.configure(inputFormat, m_surfaceView.getHolder().getSurface(), null, 0);
            } catch (Exception e) {
                e.printStackTrace();
                return false;
            }

            m_decoder.start();
            return true;
        }


        public void adjustVideoSurface() {
            try {
                if (null == m_decoder) {
                    Log.e(TAG, "Invalid decoder");
                    return;
                }

                MediaFormat fmt = m_decoder.getOutputFormat();
                if (null == fmt) {
                    Log.e(TAG, "Failed to get ouput format");
                    return;
                }

                int videoWidth = fmt.getInteger(MediaFormat.KEY_WIDTH);
                int videoHeight = fmt.getInteger(MediaFormat.KEY_HEIGHT);
                int surfaceWidth = ((View)m_surfaceView.getParent()).getWidth();
                int surfaceHeight = ((View)m_surfaceView.getParent()).getHeight();
                float scaleRatio = Math.max(((float) videoWidth / surfaceWidth), (float) videoHeight / surfaceHeight);
                int surfaceNewWidth = (int) Math.ceil((float) videoWidth / scaleRatio);
                int surfaceNewHeight = (int) Math.ceil((float) videoHeight / scaleRatio);

                RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(surfaceNewWidth, surfaceNewHeight);
                params.addRule(RelativeLayout.CENTER_IN_PARENT);
                m_surfaceView.post(() -> m_surfaceView.setLayoutParams(params));
            } catch (IllegalStateException e) {
                Log.e(TAG, "Failed to adjust the surface view size:" + e);
            }
        }

        public void convertAvccToAnnexB(byte[] data) {
            int nextNaluStart = 0;
            long currentNaluLength = 0;

            while (nextNaluStart < data.length) {
                int counter = 0;
                while (counter < m_avcConfig.nalUnitLengthFieldLength) {
                    // extract the original NALU length
                    currentNaluLength <<= 8;
                    currentNaluLength |= (long)(data[nextNaluStart + counter] & 0xff);

                    // replace the length field with NAUL separator
                    if (counter == m_avcConfig.nalUnitLengthFieldLength -1) {
                        data[nextNaluStart + counter] = 0x01;
                    } else {
                        data[nextNaluStart + counter] = 0x00;
                    }

                    // increase counter
                    counter++;
                }

                // move to next NAUL start
                nextNaluStart += m_avcConfig.nalUnitLengthFieldLength + currentNaluLength;
            }
        }
    }
    // endregion

    // region View Methods
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityApmirrorPlayerBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        long sessionId = getIntent().getLongExtra(AIRPLAY_SESSION_ID, 0);
        session = APAirPlayService.getInstance().lookupSession(sessionId);
        if (null != session) {
            session.setStopHandler(this);
            session.setMirrorHandler(this);

            synchronized (session) {
                session.notifyAll();
            }
        }
    }

    @Override
    public void onNewIntent(Intent intent) {
        super.onNewIntent(intent);
        setIntent(intent);
    }

    @Override
    public void onStart() {
        super.onStart();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (null != session) {
            session.disconnect();
        }
    }
    // endregion

    // region Methods implementation of IAirPlayMirroringHandler
    @Override
    public void on_video_stream_started() {
        Log.d(TAG, "on_video_stream_started: ");
        videoPlayer = new VideoPlayer(binding.surfaceView);
    }

    @Override
    public void on_video_stream_codec(byte[] data) {
        String AP_VIDEO_FORMAT_MIME = "video/avc";

        Log.i(TAG, "on_video_stream_codec: ");
        if (null == data || 0 == data.length) {
            return;
        }

        if (null != videoPlayer) {
            // stop and uninitialize
            videoPlayer.stop();
            videoPlayer.uninitialize();

            // initialize and start
            videoPlayer.initialize(AP_VIDEO_FORMAT_MIME, data);
            videoPlayer.start();
        }
    }

    @Override
    public void on_video_stream_data(byte[] data, long timestamp) {
        long frame_size = ((long) (data[0] & 0xff)) << 24;
        frame_size |= (((long) (data[1] & 0xff)) << 16);
        frame_size |= (((long) (data[2] & 0xff)) << 8);
        frame_size |= ((long) (data[3] & 0xff));
        Log.v(TAG, "on_video_stream_data: frame size: " + frame_size + ", length: " + data.length);
        Log.v(TAG, String.format("on_video_stream_data: %02x, %02x, %02x, %02x",
                data[0], data[1], data[2], data[3]));

        if (null != videoPlayer) {
            videoPlayer.feedVideoData(data);
        }
    }

    @Override
    public void on_video_stream_heartbeat() {
        Log.v(TAG, "on_video_stream_heartbeat: ");
    }

    @Override
    public void on_video_stream_stopped() {
        Log.i(TAG, "on_video_stream_stopped: ");
        if (null != videoPlayer) {
            videoPlayer.stop();
            videoPlayer.uninitialize();
            videoPlayer = null;
        }
    }

    @Override
    public void on_audio_set_volume(float ratio, float volume) {
        Log.i(TAG, String.format("on_audio_set_volume: ratio = %f, volume = %f", ratio, volume));
        if (null != audioPlayer) {
            audioPlayer.setVolume(ratio / 100);
        }
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
        int AP_AUDIO_SAMPLE_RATE = 44100;
        int AP_AUDIO_CHANNEL_COUNT = 2;
        int AP_AUDIO_CHANNEL_MASK = AudioFormat.CHANNEL_OUT_STEREO;

        String codecMime = "";
        MediaFormat audioFormat = MediaFormat.createAudioFormat(codecMime, AP_AUDIO_SAMPLE_RATE, AP_AUDIO_CHANNEL_COUNT);
        audioFormat.setInteger(MediaFormat.KEY_CHANNEL_MASK, AP_AUDIO_CHANNEL_MASK);
        if (format == ALAC) {
            Log.i(TAG, "on_audio_stream_started: ALAC");
            codecMime = "audio/alac";
            audioFormat.setString(MediaFormat.KEY_MIME, codecMime);
        } else if (format == AAC) {
            Log.i(TAG, "on_audio_stream_started: AAC");
            codecMime = "audio/mp4a-latm";
            audioFormat.setString(MediaFormat.KEY_MIME, codecMime);
        } else if (format == AACELD) {
            Log.i(TAG, "on_audio_stream_started: AACELD");
            codecMime = "audio/mp4a-latm";
            audioFormat.setString(MediaFormat.KEY_MIME, codecMime);
            int AP_AUDIO_AAC_PROFILE = MediaCodecInfo.CodecProfileLevel.AACObjectELD;
            audioFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, AP_AUDIO_AAC_PROFILE);
            byte[] AP_AUDIO_AAC_RAW_CONFIG = new byte[]{(byte) 0xF8, (byte) 0xE8, (byte) 0x50, (byte) 0x00};
            audioFormat.setByteBuffer("csd-0", ByteBuffer.wrap(AP_AUDIO_AAC_RAW_CONFIG));
        } else if (format == PCM) {
            Log.i(TAG, "on_audio_stream_started: PCM");
            return;
        } else {
            Log.i(TAG, "on_audio_stream_started: Unknown");
            return;
        }

        audioPlayer = new AudioPlayer();
        if (!audioPlayer.initialize(codecMime, audioFormat)) {
            Log.e(TAG,"Failed to initialize the audio player");
            audioPlayer = null;
            return;
        }
        audioPlayer.start();
    }

    @Override
    public void on_audio_stream_data(byte[] data, long timestamp) {
        Log.v(TAG, "on_audio_stream_data: " + data.length + ", timestamp: " + timestamp);
        if (null != audioPlayer) {
            audioPlayer.feedAudioData(data);
        }
    }

    @Override
    public void on_audio_stream_stopped() {
        Log.i(TAG, "on_audio_stream_stopped: ");
        if (null != audioPlayer) {
            audioPlayer.stop();
            audioPlayer.uninitialize();
            audioPlayer = null;
        }
    }

    @Override
    public void onSessionStop() {
        this.finish();
    }
    // endregion
}