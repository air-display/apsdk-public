/* 
 *  File: ap_casting_event_connection_manager.h
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
#include <map>
#include <memory>
#include <string>

#include <network/xtxp_connection_base.h>

using namespace aps::network;

namespace aps {
namespace service {
/// <summary>
/// Represents the manager of the reversed HTTP connections.
/// </summary>
class ap_casting_event_connection_manager {
  typedef std::map<std::string, xtxp_connection_base_weak_ptr> event_connection_map;

public:
  static ap_casting_event_connection_manager &get();

  void insert(const std::string &id, xtxp_connection_base_weak_ptr p);
  void remove(const std::string &id);
  xtxp_connection_base_weak_ptr get(const std::string &id);

protected:
  ap_casting_event_connection_manager();
  ~ap_casting_event_connection_manager();

private:
  event_connection_map event_connection_map_;
  std::mutex mtx_;
};

} // namespace service
} // namespace aps
