/* 
 *  File: ap_casting_content_parser.h
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

#pragma once
#include <ap_types.h>
#include <string>

namespace aps {

namespace ap_casting_content_parser {

bool get_volume_value(float &value, const char *content);

bool get_progress_values(uint64_t &start, uint64_t &current, uint64_t &end, const char *content);

bool get_play_parameters(std::string &location, float &start, const char *content);

bool get_scrub_position(float &postion, const char *content);

bool get_rate_value(float &rate, const char *content);

void get_user_agent_version(std::string &agent, aps::agent_version_t &version, const char *content);
} // namespace ap_casting_content_parser
} // namespace aps
