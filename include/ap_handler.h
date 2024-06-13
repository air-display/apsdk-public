/* 
 *  File: ap_handler.h
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

#ifndef AP_HANDLER_H
#define AP_HANDLER_H
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <ap_export.h>
#include <ap_session.h>
#include <ap_types.h>

namespace aps {
/// <summary>
///
/// </summary>
class APS_EXPORT ap_handler {
public:
  /// <summary>
  ///
  /// </summary>
  virtual ~ap_handler(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="session"></param>
  virtual void on_session_begin(aps::ap_session_ptr session) = 0;

  /// <summary>
  ///
  /// </summary>
  virtual void on_session_end(const uint64_t session_id) = 0;
};

/// <summary>
///
/// </summary>
typedef std::shared_ptr<ap_handler> ap_handler_ptr;
} // namespace aps
#endif // AP_HANDLER_H
