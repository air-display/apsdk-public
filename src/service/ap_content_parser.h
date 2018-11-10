#pragma once
#include <string>

namespace aps { namespace ap_content_parser {

    bool get_volume(
        float& value, 
        const char* content);

    bool get_progress(
        uint64_t& start,
        uint64_t& current,
        uint64_t& end,
        const char* content);

    bool get_play_parameters(
        std::string& location,
        float& start,
        const char* content);
} }

