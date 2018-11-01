#pragma once
#include <memory>
#include <asio.hpp>
#include <utils/logger.h>
#include <utils/packing.h>

namespace aps { namespace network {
    /// <summary>
    /// 
    /// </summary>
    enum rtp_payload_type_e
    {
        timing_query = 82,
        timing_reply = 83,

        ctrl_timing_sync = 84,
        ctrl_retransmit_request = 85,
        ctrl_retransmit_reply = 86,
        
        audio_data = 96,
    };
    typedef rtp_payload_type_e rtp_payload_type_t;

    /// <summary>
    /// 
    /// </summary>
    PACKED(struct rtp_packet_header_s
    {
        uint8_t csrc_count : 4;            /* CSRC count */
        uint8_t extension : 1;             /* header extension flag */
        uint8_t padding : 1;               /* padding flag */
        uint8_t version : 2;               /* protocol version */
        uint8_t payload_type : 7;          /* payload type */
        uint8_t marker : 1;                /* marker bit */
        uint16_t sequence;                 /* sequence number */
        uint32_t timestamp;                /* timestamp */

        rtp_packet_header_s()
        {
            memset(this, 0, sizeof(rtp_packet_header_s));
            version = 2;
        }
    });
    typedef rtp_packet_header_s rtp_packet_header_t;

    PACKED(struct rtp_timming_sync_packet_s
    {
        rtp_packet_header_t header;
        uint64_t original_timestamp;
        uint64_t receive_timestamp;
        uint64_t transmit_timestamp;

        rtp_timming_sync_packet_s()
        {
            original_timestamp = 0;
            receive_timestamp = 0;
            transmit_timestamp = 0;
        }
    });
    typedef rtp_timming_sync_packet_s rtp_timming_sync_packet_t;
} }
