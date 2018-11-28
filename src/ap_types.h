#pragma once
#include <stdint.h>
#include <string.h>
#include <utils/packing.h>

namespace aps {
/// <summary>
///
/// </summary>
enum rtp_payload_type_e {
  rtp_timing_query = 82,
  rtp_timing_reply = 83,

  rtp_ctrl_timing_sync = 84,
  rtp_ctrl_retransmit_request = 85,
  rtp_ctrl_retransmit_reply = 86,

  rtp_audio_data = 96,
};
typedef rtp_payload_type_e rtp_payload_type_t;

/// <summary>
///
/// </summary>
PACKED(struct rtp_packet_header_s {
  uint8_t csrc_count : 4;   /* CSRC count */
  uint8_t extension : 1;    /* header extension flag */
  uint8_t padding : 1;      /* padding flag */
  uint8_t version : 2;      /* protocol version */
  uint8_t payload_type : 7; /* payload type */
  uint8_t marker : 1;       /* marker bit */
  uint16_t sequence;        /* sequence number */
  uint32_t timestamp;       /* timestamp */

  rtp_packet_header_s() {
    memset(this, 0, sizeof(rtp_packet_header_s));
    version = 2;
  }
});
typedef rtp_packet_header_s rtp_packet_header_t;

PACKED(struct rtp_timming_sync_packet_s
       : public rtp_packet_header_t {
         uint64_t original_timestamp;
         uint64_t receive_timestamp;
         uint64_t transmit_timestamp;

         rtp_timming_sync_packet_s() : rtp_packet_header_t() {
           original_timestamp = 0;
           receive_timestamp = 0;
           transmit_timestamp = 0;
         }
       });
typedef rtp_timming_sync_packet_s rtp_timming_sync_packet_t;

PACKED(struct rtp_control_sync_packet_s
       : public rtp_packet_header_t {
         uint64_t current_ntp_time;
         uint32_t next_packet_time;

         rtp_control_sync_packet_s() : rtp_packet_header_t() {
           current_ntp_time = 0;
           next_packet_time = 0;
         }
       });
typedef rtp_control_sync_packet_s rtp_control_sync_packet_t;

PACKED(struct rtp_control_retransmit_packet_s
       : public rtp_packet_header_t {
         uint16_t lost_packet_start;
         uint16_t lost_packet_count;

         rtp_control_retransmit_packet_s() : rtp_packet_header_t() {
           lost_packet_start = 0;
           lost_packet_count = 0;
         }
       });
typedef rtp_control_retransmit_packet_s rtp_control_retransmit_packet_t;

PACKED(struct rtp_audio_data_packet_s
       : public rtp_packet_header_t {
         uint32_t ssrc;
         uint8_t payload[];

         rtp_audio_data_packet_s() : rtp_packet_header_t() { ssrc = 0; }
       });
typedef rtp_audio_data_packet_s rtp_audio_data_packet_t;

enum sms_payload_type_e {
  sms_video_data = 0,
  sms_video_codec = 1,
  sms_payload_5 = 5,
  sms_payload_4096 = 4096,
};
typedef sms_payload_type_e sms_payload_type_t;

/// <summary>
/// Screen Mirroring Stream(SMS) packet header.
/// </summary>
PACKED(struct sms_packet_header_s {
  uint32_t payload_size;
  uint16_t payload_type;
  uint16_t reserverd;
  uint64_t timestamp;
  uint8_t padding[112];
});
typedef sms_packet_header_s sms_packet_header_t;

// 1 byte    1       version
// 1 byte    100     profile (high)
// 1 byte    0xc0    compatibility
// 1 byte    40      level (4.0)
// 6 bits    0x3f    reserved
// 2 bits    3       NAL units length size - 1
// 3 bits    0x7     reserved
// 5 bits    1       number of SPS
// 2 bytes   16      length of SPS
// 16 bytes          Sequence parameter set
// 1 byte    1       number of PPS
// 2 bytes   4       length of PPS
// 4 bytes           Picture parameter set
PACKED(struct sms_video_codec_packet_s
       : public sms_packet_header_t {
         uint8_t version;
         uint8_t profile;
         uint8_t compatibility;
         uint8_t level;
         uint8_t reserved0 : 6;
         uint8_t nallength : 2;
         uint8_t reserved1 : 3;
         uint8_t sps_count : 5;
         uint16_t sps_length;
         uint8_t reserved2[16];
         uint8_t pps_count;
         uint16_t pps_length;
         uint8_t padding[128];
       });
typedef sms_video_codec_packet_s sms_video_codec_packet_t;

PACKED(struct sms_video_data_packet_s
       : public sms_packet_header_t { uint8_t payload[]; });
typedef sms_video_data_packet_s sms_video_data_packet_t;

PACKED(struct sms_video_5_packet_s
       : public sms_packet_header_t { uint8_t payload[]; });
typedef sms_video_5_packet_s sms_video_5_packet_t;

PACKED(struct sms_video_4096_packet_s
       : public sms_packet_header_t { uint8_t payload[]; });
typedef sms_video_4096_packet_s sms_video_4096_packet_t;

}  // namespace aps
