package com.medialab.airplay;

public class MirroringVideoCodec {
    public byte version;
    public byte profile;
    public byte compatibility;
    public byte level;
    public byte NALLength;
    public byte spsCount;

    public byte spsLength;
    public byte[] reserved2;

    public byte ppsCount;

    public short ppsLength;

    public MirroringVideoCodec() {
    }
}
