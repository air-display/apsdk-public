/* 
 *  File: net_service.cpp
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

#include "net_service.h"
#include "net_service_impl.h"

net_service::net_service(const std::string &type) : impl_(implementation::get(type)) {}

net_service::~net_service() {
  if (impl_)
    impl_.reset();
}

void net_service::add_txt_record(const std::string &k, const std::string &v) {
  if (impl_)
    impl_->add_txt_record(k, v);
}

void net_service::add_txt_record(const std::string &k, const int &v) {
  if (impl_)
    impl_->add_txt_record(k, std::to_string(v));
}

bool net_service::publish(const std::string &name, const uint16_t port) {
  if (impl_)
    return impl_->publish(name, port);

  return false;
}

void net_service::suppress() {
  if (impl_)
    return impl_->suppress();
}
