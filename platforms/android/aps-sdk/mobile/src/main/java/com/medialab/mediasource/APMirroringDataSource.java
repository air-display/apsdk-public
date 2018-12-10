package com.medialab.mediasource;

import android.net.Uri;
import android.support.annotation.Nullable;

import com.google.android.exoplayer2.upstream.BaseDataSource;
import com.google.android.exoplayer2.upstream.DataSpec;

import java.io.IOException;

public class APMirroringDataSource extends BaseDataSource {
    /**
     * Creates base data source.
     *
     */
    protected APMirroringDataSource() {
        super(true);
    }

    @Override
    public long open(DataSpec dataSpec) throws IOException {
        return 0;
    }

    @Override
    public int read(byte[] buffer, int offset, int readLength) throws IOException {
        return 0;
    }

    @Nullable
    @Override
    public Uri getUri() {
        return null;
    }

    @Override
    public void close() throws IOException {

    }
}
