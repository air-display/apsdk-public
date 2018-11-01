#pragma once
#include <vector>
#include <asio.hpp>
#include <utils/packing.h>

namespace aps { namespace service { namespace audio { namespace details {
    enum payload_type_e
    {
        payload_timing_request = 82,
        payload_timing_reply = 83,
        payload_timing_sync = 84,
        payload_retransmit_request = 85,
        payload_retransmit_reply = 86,
        payload_audio_data = 96,
    };
    typedef payload_type_e payload_type_t;

    PACKED(struct packet_header
    {
        static const int LENGTH = 128;
        uint32_t version : 2;               /* protocol version */
        uint32_t padding : 1;               /* padding flag */
        uint32_t extension : 1;             /* header extension flag */
        uint32_t csrc_count : 4;            /* CSRC count */
        uint32_t marker : 1;                /* marker bit */
        uint32_t payload_type : 7;          /* payload type */
        uint32_t sequence : 16;             /* sequence number */
        uint32_t timestamp;                 /* timestamp */
        uint32_t ssrc;                      /* synchronization source identifier */
    });
} } } }