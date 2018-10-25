#pragma once
#include <memory>
#include "../utils/packing.h"

namespace aps {
    /// <summary>
    /// 
    /// </summary>
    typedef enum stream_type_e {
        audio = 96,
        video = 110,
    } stream_type_t;

    /// <summary>
    /// 
    /// </summary>
    PACKED(struct pair_verify_header_s {
        uint8_t is_first_frame;
        uint8_t reserved0;
        uint8_t reserved1;
        uint8_t reserved2;
    });
    
    typedef pair_verify_header_s pair_verify_header_t;
}
