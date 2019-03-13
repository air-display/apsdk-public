#ifndef AP_TYPES_H
#define AP_TYPES_H
#pragma once
#pragma once

#include <cstdint>
#include <cstring>

#ifdef __GNUC__
#define PACKED(type_to_pack) type_to_pack __attribute__((__packed__))
#else
#define PACKED(type_to_pack)                                                   \
  __pragma(pack(push, 1)) type_to_pack __pragma(pack(pop))
#endif

namespace aps {
/// <summary>
///
/// </summary>
struct agent_version_s {
  uint16_t major;
  uint16_t minor;
  uint16_t revision;
  uint16_t build;
};
typedef agent_version_s agent_version_t;

/// <summary>
///
/// </summary>
enum rtp_payload_type_e {
  // from timing sync port
  rtp_timing_query = 82,
  rtp_timing_reply = 83,

  // from control port
  rtp_ctrl_timing_sync = 84,
  rtp_ctrl_retransmit_request = 85, // s -> c
  rtp_ctrl_retransmit_reply = 86,

  // from data port
  rtp_audio_data = 96,
};
typedef rtp_payload_type_e rtp_payload_type_t;

/// <summary>
///
/// </summary>
enum audio_data_format_e {
  audio_format_pcm = 0,

  /// <summary>
  /// 96 AppleLossless
  /// 96 352 0 16 40 10 14 2 255 0 0 44100
  /// </summary>
  audio_format_alac = 1,

  /// <summary>
  /// 96 mpeg4-generic/44100/2
  /// 96 mode=AAC-main; constantDuration=1024
  /// </summary>
  audio_format_aac = 2,

  /// <summary>
  /// 96 mpeg4-generic/44100/2
  /// 96 mode=AAC-eld; constantDuration=480
  /// </summary>
  audio_format_aac_eld = 3
};
typedef audio_data_format_e audio_data_format_t;

/// <summary>
///
/// </summary>
PACKED(struct rtp_packet_header_s {
  uint8_t csrc_count : 4; /* CSRC count */
  uint8_t extension : 1;  /* header extension flag */
  uint8_t padding : 1;    /* padding flag */
  uint8_t version : 2;    /* protocol version */

  uint8_t payload_type : 7; /* payload type */
  uint8_t marker : 1;       /* marker bit */

  uint16_t sequence;  /* sequence number */
  uint32_t timestamp; /* timestamp, unit is frequency rate */

  rtp_packet_header_s() {
    memset(this, 0, sizeof(rtp_packet_header_s));
    version = 2;
  }
});
typedef rtp_packet_header_s rtp_packet_header_t;

PACKED(struct rtp_audio_data_packet_s
       : public rtp_packet_header_t {
         uint32_t ssrc;
         uint8_t payload[0];

         rtp_audio_data_packet_s() : rtp_packet_header_t() { ssrc = 0; }
       });
typedef rtp_audio_data_packet_s rtp_audio_data_packet_t;

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

PACKED(struct rtp_control_retransmit_request_packet_s
       : public rtp_packet_header_t {
         uint16_t lost_packet_start;
         uint16_t lost_packet_count;

         rtp_control_retransmit_request_packet_s() : rtp_packet_header_t() {
           lost_packet_start = 0;
           lost_packet_count = 0;
         }
       });
typedef rtp_control_retransmit_request_packet_s
    rtp_control_retransmit_request_packet_t;

PACKED(struct rtp_control_retransmit_reply_packet_s {
  uint8_t csrc_count : 4;   /* CSRC count */
  uint8_t extension : 1;    /* header extension flag */
  uint8_t padding : 1;      /* padding flag */
  uint8_t version : 2;      /* protocol version */
  uint8_t payload_type : 7; /* payload type */
  uint8_t marker : 1;       /* marker bit */
  uint16_t sequence;        /* sequence number */
  rtp_audio_data_packet_t data_packet;

  rtp_control_retransmit_reply_packet_s() : data_packet() {
    memset(this, 0, sizeof(rtp_control_retransmit_reply_packet_s));
  }
});
typedef rtp_control_retransmit_reply_packet_s
    rtp_control_retransmit_reply_packet_t;

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
/// <remarks>
/// All fields are little-endian
/// </remarks>
PACKED(struct sms_packet_header_s {
  uint32_t payload_size;
  uint16_t payload_type;
  uint16_t reserverd;
  uint64_t timestamp; // NTP timestamp
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
PACKED(struct avc_decoder_config_record_s {
  uint8_t version;
  uint8_t profile;
  uint8_t compatibility;
  uint8_t level;
  uint8_t nallength : 2;
  uint8_t reserved0 : 6;
  uint8_t sps_count : 5;
  uint8_t reserved1 : 3;
  uint8_t start[0];

  // struct {
  //  uint16_t sps_length;
  //  // spsNALUnit size = sps_length;
  //}[sps_count];

  // uint8_t pps_count;
  // struct {
  //  uint16_t pps_length;
  //  // ppsNALUnit size = pps_length;
  //}[pps_count];
});
typedef avc_decoder_config_record_s avc_decoder_config_record_t;

PACKED(struct sms_video_codec_packet_s
       : public sms_packet_header_t {
         union {
           uint8_t payload[0];
           avc_decoder_config_record_t decord_record;
         };
       });
typedef sms_video_codec_packet_s sms_video_codec_packet_t;

PACKED(struct sms_video_data_packet_s
       : public sms_packet_header_t { uint8_t payload[0]; });
typedef sms_video_data_packet_s sms_video_data_packet_t;

PACKED(struct sms_video_5_packet_s
       : public sms_packet_header_t { uint8_t payload[0]; });
typedef sms_video_5_packet_s sms_video_5_packet_t;

PACKED(struct sms_video_4096_packet_s
       : public sms_packet_header_t { uint8_t payload[0]; });
typedef sms_video_4096_packet_s sms_video_4096_packet_t;

} // namespace aps

#endif // AP_TYPES_H
