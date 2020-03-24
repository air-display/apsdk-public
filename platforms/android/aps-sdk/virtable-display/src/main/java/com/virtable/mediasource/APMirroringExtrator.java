package com.virtable.mediasource;

import com.google.android.exoplayer2.C;
import com.google.android.exoplayer2.extractor.Extractor;
import com.google.android.exoplayer2.extractor.ExtractorInput;
import com.google.android.exoplayer2.extractor.ExtractorOutput;
import com.google.android.exoplayer2.extractor.ExtractorsFactory;
import com.google.android.exoplayer2.extractor.PositionHolder;
import com.google.android.exoplayer2.extractor.SeekMap;
import com.google.android.exoplayer2.extractor.ts.TsPayloadReader;
import com.google.android.exoplayer2.util.ParsableByteArray;

import java.io.IOException;

public class APMirroringExtrator implements Extractor {
  public static final ExtractorsFactory FACTORY = () -> new Extractor[]{new APMirroringExtrator()};

  /**
   * The maximum number of bytes to search when sniffing, excluding ID3 information, before giving
   * up.
   */
  private static final int MAX_SYNC_FRAME_SIZE = 2786;

  private final long firstSampleTimestampUs;
  private final APMirroringH264Reader reader;
  private final ParsableByteArray sampleData;
  private boolean startedPacket;
  private ExtractorOutput extractorOutput;

  public APMirroringExtrator() {
    this(0);
  }

  public APMirroringExtrator(long firstSampleTimestampUs) {
    this.firstSampleTimestampUs = firstSampleTimestampUs;
    reader = new APMirroringH264Reader(true, false);
    sampleData = new ParsableByteArray(MAX_SYNC_FRAME_SIZE);
  }

  @Override
  public boolean sniff(ExtractorInput input) throws IOException, InterruptedException {
    return true;
  }

  @Override
  public void init(ExtractorOutput output) {
    extractorOutput = output;
    reader.createTracks(extractorOutput, new TsPayloadReader.TrackIdGenerator(0, 1));
    extractorOutput.endTracks();
    extractorOutput.seekMap(new SeekMap.Unseekable(C.TIME_UNSET));
  }

  @Override
  public int read(ExtractorInput input, PositionHolder seekPosition) throws IOException, InterruptedException {
    int bytesRead = input.read(sampleData.data, 0, MAX_SYNC_FRAME_SIZE);
    if (bytesRead == C.RESULT_END_OF_INPUT) {
      return RESULT_END_OF_INPUT;
    }

    // Feed whatever data we have to the reader, regardless of whether the read finished or not.
    sampleData.setPosition(0);
    sampleData.setLimit(bytesRead);

    if (!startedPacket) {
      // Pass data to the reader as though it's contained within a single infinitely long packet.
      reader.packetStarted(firstSampleTimestampUs, true);
      startedPacket = true;
    }
    // TODO: Make it possible for the reader to consume the dataSource directly, so that it becomes
    // unnecessary to copy the data through packetBuffer.
    reader.consume(sampleData);
    return RESULT_CONTINUE;
  }

  @Override
  public void seek(long position, long timeUs) {
  }

  @Override
  public void release() {
  }
}
