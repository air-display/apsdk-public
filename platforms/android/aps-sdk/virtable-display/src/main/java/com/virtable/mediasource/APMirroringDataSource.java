package com.virtable.mediasource;

import android.net.Uri;
import android.support.annotation.Nullable;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.upstream.BaseDataSource;
import com.google.android.exoplayer2.upstream.DataSpec;

import java.io.EOFException;
import java.io.IOException;
import java.io.PipedInputStream;

public class APMirroringDataSource extends BaseDataSource {
  private long bytesToRead = 0;
  private long bytesRead = 0;
  private PipedInputStream inputStream;

  /**
   * Creates base data source.
   */
  protected APMirroringDataSource() {
    super(true);
    inputStream = new PipedInputStream();
  }

  @Override
  public long open(DataSpec dataSpec) throws IOException {
    Uri uri = dataSpec.uri;
    inputStream.connect(null);
    return 0;
  }

  @Override
  public int read(byte[] buffer, int offset, int readLength) throws IOException {
    if (readLength == 0) {
      return 0;
    }
    if (bytesToRead != C.LENGTH_UNSET) {
      long bytesRemaining = bytesToRead - bytesRead;
      if (bytesRemaining == 0) {
        return C.RESULT_END_OF_INPUT;
      }
      readLength = (int) Math.min(readLength, bytesRemaining);
    }

    int read = inputStream.read(buffer, offset, readLength);
    if (read == -1) {
      if (bytesToRead != C.LENGTH_UNSET) {
        // End of stream reached having not read sufficient data.
        throw new EOFException();
      }
      return C.RESULT_END_OF_INPUT;
    }

    bytesRead += read;
    bytesTransferred(read);
    return read;
  }

  @Nullable
  @Override
  public Uri getUri() {
    return null;
  }

  @Override
  public void close() throws IOException {
    inputStream.close();
  }
}
