/* 
 *  File: net_service.h
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

#ifndef MDNS_H
#define MDNS_H
#pragma once

#include <cstdint>
#include <memory>
#include <string>

class net_service {
public:
  net_service(const std::string &type);

  ~net_service();

  void add_txt_record(const std::string &k, const std::string &v);

  void add_txt_record(const std::string &k, const int &v);

  bool publish(const std::string &name, const uint16_t port);

  void suppress();

private:
  class implementation;
  std::unique_ptr<implementation> impl_;

  class net_service_impl;
};

#endif // MDNS_H
