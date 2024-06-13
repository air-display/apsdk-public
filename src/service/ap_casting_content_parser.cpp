/* 
 *  File: ap_casting_content_parser.cpp
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#include <regex>

#include <service/ap_casting_content_parser.h>

bool aps::ap_casting_content_parser::get_volume_value(float &value, const char *content) {
  static std::regex pattern("volume: ([-+]?[0-9]+\\.[0-9]+)");
  std::cmatch groups;

  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 1) {
      value = std::strtof(groups.str(1).c_str(), nullptr);
      return true;
    }
  }

  return false;
}

bool aps::ap_casting_content_parser::get_progress_values(uint64_t &start, uint64_t &current, uint64_t &end,
                                                         const char *content) {
  static std::regex pattern("progress: ([0-9]+)\\/([0-9]+)\\/([0-9]+)");
  std::cmatch groups;

  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 3) {
      start = std::strtoll(groups.str(1).c_str(), nullptr, 10);
      current = std::strtoll(groups.str(2).c_str(), nullptr, 10);
      end = std::strtoll(groups.str(3).c_str(), nullptr, 10);
      return true;
    }
  }

  return false;
}

bool aps::ap_casting_content_parser::get_play_parameters(std::string &location, float &start, const char *content) {
  static std::regex pattern("Content-Location: (.*)\\nStart-Position: ([-+]?[0-9]+\\.[0-9]+)");
  std::cmatch groups;

  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 2) {
      location = groups.str(1);
      start = std::strtof(groups.str(2).c_str(), nullptr);
      return true;
    }
  }

  return false;
}

bool aps::ap_casting_content_parser::get_scrub_position(float &postion, const char *content) {
  static std::regex pattern(R"(\/scrub\?position=([0-9]+\.[0-9]+))");
  std::cmatch groups;

  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 1) {
      postion = std::strtof(groups.str(1).c_str(), nullptr);
      return true;
    }
  }

  return false;
}

bool aps::ap_casting_content_parser::get_rate_value(float &rate, const char *content) {
  static std::regex pattern(R"(\/rate\?value=([0-9]+\.[0-9]+))");
  std::cmatch groups;

  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 1) {
      rate = std::strtof(groups.str(1).c_str(), nullptr);
      return true;
    }
  }

  return false;
}

void aps::ap_casting_content_parser::get_user_agent_version(std::string &agent, aps::agent_version_t &version,
                                                            const char *content) {
  static std::regex pattern(R"((.*)\/([0-9]+)(?:\.([0-9]+)(?:\.([0-9]+)(?:\.([0-9]+))?)?)?)");
  std::cmatch groups;
  memset(&version, 0, sizeof(aps::agent_version_t));
  if (std::regex_search(content, groups, pattern)) {
    if (groups.size() > 1) {
      agent = groups.str(1);
    }
    if (groups.size() > 2) {
      version.major = (uint16_t)std::strtol(groups.str(2).c_str(), nullptr, 10);
    }
    if (groups.size() > 3) {
      version.minor = (uint16_t)std::strtol(groups.str(3).c_str(), nullptr, 10);
    }
    if (groups.size() > 4) {
      version.revision = (uint16_t)std::strtol(groups.str(4).c_str(), nullptr, 10);
    }
    if (groups.size() > 5) {
      version.build = (uint16_t)std::strtol(groups.str(5).c_str(), nullptr, 10);
    }
  }
}
