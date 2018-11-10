#include <regex>
#include "ap_content_parser.h"

bool aps::ap_content_parser::get_volume(float& value, const char* content)
{
    static std::regex pattern("volume: ([-+]?[0-9]+\\.[0-9]+)");
    std::cmatch groups;

    if (std::regex_search(content, groups, pattern)) {
        if (groups.size() > 1) {
            value = std::strtof(groups.str(1).c_str(), 0);
            return true;
        }
    }

    return false;
}

bool aps::ap_content_parser::get_progress(uint64_t& start, uint64_t& current, uint64_t& end, const char* content)
{
    static std::regex pattern("progress: ([0-9]+)\\/([0-9]+)\\/([0-9]+)");
    std::cmatch groups;

    if (std::regex_search(content, groups, pattern)) {
        if (groups.size() > 3) {
            start = std::strtoll(groups.str(1).c_str(), 0, 10);
            current = std::strtoll(groups.str(2).c_str(), 0, 10);
            end = std::strtoll(groups.str(3).c_str(), 0, 10);
            return true;
        }
    }

    return false;
}

bool aps::ap_content_parser::get_play_parameters(std::string& location, float& start, const char* content)
{
    static std::regex pattern("Content-Location: (.*)\\nStart-Position: ([-+]?[0-9]+\\.[0-9]+)");
    std::cmatch groups;

    if (std::regex_search(content, groups, pattern)) {
        if (groups.size() > 2) {
            location = groups.str(1);
            start = std::strtof(groups.str(2).c_str(), 0);
            return true;
        }
    }

    return false;
}
