package com.medialab.airplay;

public class PlaybackInfo {
    public String uuid;
    public int stallCount;
    public double duration;
    public float position;
    public double rate;
    public boolean readyToPlay;
    public boolean playbackBufferEmpty;
    public boolean playbackBufferFull;
    public boolean playbackLikelyToKeepUp;

    PlaybackInfo() {
        uuid = "";
        stallCount = 0;
        duration = 0;
        position = 0;
        rate = 0;
        readyToPlay = false;
        playbackBufferEmpty = true;
        playbackBufferFull = false;
        playbackLikelyToKeepUp = false;
    }
}
