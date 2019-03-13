#pragma once
#include <ap_types.h>
#include <string>

namespace aps {

namespace ap_content_parser {

bool get_volume_value(float &value, const char *content);

bool get_progress_values(uint64_t &start, uint64_t &current, uint64_t &end,
                         const char *content);

bool get_play_parameters(std::string &location, float &start,
                         const char *content);

bool get_scrub_position(float &postion, const char *content);

bool get_rate_value(float &rate, const char *content);

void get_user_agent_version(std::string &agent, aps::agent_version_t &version,
                            const char *content);
} // namespace ap_content_parser
} // namespace aps
