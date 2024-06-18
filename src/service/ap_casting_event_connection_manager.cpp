/* 
 *  File: ap_casting_event_connection_manager.cpp
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

#include <service/ap_casting_event_connection_manager.h>

#include <utility>

namespace aps {
namespace service {
aps::service::ap_casting_event_connection_manager &ap_casting_event_connection_manager::get() {
  static ap_casting_event_connection_manager s_instance;
  return s_instance;
}

void ap_casting_event_connection_manager::insert(const std::string &id, xtxp_connection_base_weak_ptr p) {
  std::lock_guard<std::mutex> l(mtx_);
  event_connection_map_[id] = std::move(p);
}

void ap_casting_event_connection_manager::remove(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = event_connection_map_.find(id);
  if (it != event_connection_map_.end()) {
    event_connection_map_.erase(it);
  }
}

xtxp_connection_base_weak_ptr ap_casting_event_connection_manager::get(const std::string &id) {
  std::lock_guard<std::mutex> l(mtx_);
  auto it = event_connection_map_.find(id);
  if (it != event_connection_map_.end()) {
    return it->second;
  }
  return xtxp_connection_base_weak_ptr();
}

ap_casting_event_connection_manager::ap_casting_event_connection_manager() = default;

ap_casting_event_connection_manager::~ap_casting_event_connection_manager() = default;

} // namespace service
} // namespace aps
