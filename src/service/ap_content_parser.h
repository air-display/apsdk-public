#pragma once
#include <string>

namespace aps
{

struct agent_version_s
{
  uint16_t major;
  uint16_t minor;
  uint16_t revision;
  uint16_t build;
};
typedef agent_version_s agent_version_t;

namespace ap_content_parser
{

bool get_volume_value(float &value, const char *content);

bool get_progress_values(uint64_t &start, uint64_t &current, uint64_t &end,
                         const char *content);

bool get_play_parameters(std::string &location, float &start,
                         const char *content);

bool get_scrub_position(float &postion, const char *content);

bool get_rate_value(float &rate, const char *content);

void get_user_agent_version(std::string &agent, agent_version_t &version, const char *content);
} // namespace ap_content_parser
} // namespace aps
