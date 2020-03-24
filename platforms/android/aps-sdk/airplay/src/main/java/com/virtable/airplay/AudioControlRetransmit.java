package com.virtable.airplay;

class AudioControlRetransmit {
  public short sequence;
  public int timestamp;
  public long lost_packet_start;
  public int lost_packet_count;

  public AudioControlRetransmit() {
  }
}
