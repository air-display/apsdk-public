#pragma once
#include <vector>
#include <asio.hpp>
#include <utils/packing.h>

namespace aps { namespace service { namespace details {
    enum payload_type
    {
        mirror_payload_video = 0,
        mirror_payload_codec = 1,
        mirror_payload_5 = 5,
        mirror_payload_4096 = 4096,
    };

    PACKED(struct packet_header
    {
        static const int LENGTH = 128;
        uint32_t payload_size;
        uint16_t payload_type;
        uint16_t reserverd;
        uint64_t timestamp;
        uint8_t padding[128];
    });

    //1 byte    1       version
    //1 byte    100     profile (high)
    //1 byte    0xc0	compatibility
    //1 byte    40      level (4.0)
    //6 bits    0x3f	reserved
    //2 bits    3       NAL units length size - 1
    //3 bits    0x7     reserved
    //5 bits    1       number of SPS
    //2 bytes   16      length of SPS
    //16 bytes  бн       Sequence parameter set
    //1 byte    1       number of PPS
    //2 bytes   4       length of PPS
    //4 bytes   бн       Picture parameter set
    PACKED(struct video_codec
    {
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

    typedef std::vector<uint8_t> packet_payload;

    PACKED(struct stream_packet
    {
        packet_header header;
        packet_payload payload;

        stream_packet() {};
        ~stream_packet() {};
    });
} } }