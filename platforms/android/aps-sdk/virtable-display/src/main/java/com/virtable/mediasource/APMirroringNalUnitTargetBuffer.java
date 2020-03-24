package com.virtable.mediasource;

import com.google.android.exoplayer2.util.Assertions;

import java.util.Arrays;

/**
 * A buffer that fills itself with data corresponding to a specific NAL unit, as it is
 * encountered in the stream.
 */
/* package */ final class APMirroringNalUnitTargetBuffer {
  public static final int START_CODE_LENGTH = 4;

  private final int targetType;
  public byte[] nalData;
  public int nalLength;
  private boolean isFilling;
  private boolean isCompleted;

  public APMirroringNalUnitTargetBuffer(int targetType, int initialCapacity) {
    this.targetType = targetType;

    // Initialize data with a start code in the first three bytes.
    nalData = new byte[START_CODE_LENGTH + initialCapacity];
    nalData[2] = 1;
  }

  /**
   * Resets the buffer, clearing any data that it holds.
   */
  public void reset() {
    isFilling = false;
    isCompleted = false;
  }

  /**
   * Returns whether the buffer currently holds a complete NAL unit of the target type.
   */
  public boolean isCompleted() {
    return isCompleted;
  }

  /**
   * Called to indicate that a NAL unit has started.
   *
   * @param type The type of the NAL unit.
   */
  public void startNalUnit(int type) {
    Assertions.checkState(!isFilling);
    isFilling = type == targetType;
    if (isFilling) {
      // Skip the three byte start code when writing data.
      nalLength = START_CODE_LENGTH;
      isCompleted = false;
    }
  }

  /**
   * Called to pass stream data. The data passed should not include the 3 byte start code.
   *
   * @param data   Holds the data being passed.
   * @param offset The offset of the data in {@code data}.
   * @param limit  The limit (exclusive) of the data in {@code data}.
   */
  public void appendToNalUnit(byte[] data, int offset, int limit) {
    if (!isFilling) {
      return;
    }
    int readLength = limit - offset;
    if (nalData.length < nalLength + readLength) {
      nalData = Arrays.copyOf(nalData, (nalLength + readLength) * 2);
    }
    System.arraycopy(data, offset, nalData, nalLength, readLength);
    nalLength += readLength;
  }

  /**
   * Called to indicate that a NAL unit has ended.
   *
   * @param discardPadding The number of excess bytes that were passed to
   *                       {@link #appendToNalUnit(byte[], int, int)}, which should be discarded.
   * @return Whether the ended NAL unit is of the target type.
   */
  public boolean endNalUnit(int discardPadding) {
    if (!isFilling) {
      return false;
    }
    nalLength -= discardPadding;
    isFilling = false;
    isCompleted = true;
    return true;
  }

}

