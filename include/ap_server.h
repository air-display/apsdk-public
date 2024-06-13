/* 
 *  File: ap_server.h
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

#ifndef AP_SERVER_H
#define AP_SERVER_H
#pragma once

#include <memory>
#if __ANDROID__
#include <jni.h>
#endif

#include <ap_config.h>
#include <ap_export.h>
#include <ap_handler.h>

namespace aps {
/// <summary>
///
/// </summary>
class APS_EXPORT ap_server {
public:
  /// <summary>
  ///
  /// </summary>
  explicit ap_server();

  /// <summary>
  ///
  /// </summary>
  ~ap_server();

  /// <summary>
  ///
  /// </summary>
  /// <param name="config"></param>
  void set_config(ap_config_ptr &config);

  /// <summary>
  ///
  /// </summary>
  /// <param name="hanlder"></param>
  void set_handler(ap_handler_ptr &hanlder);

  /// <summary>
  ///
  /// </summary>
  /// <returns></returns>
  bool start();

  /// <summary>
  ///
  /// </summary>
  void stop();

  /// <summary>
  ///
  /// </summary>
  uint16_t get_service_port();

#if __ANDROID__
  /// <summary>
  ///
  /// </summary>
  /// <param name="vm"></param>
  static void setJavaVM(JavaVM *vm);
#endif

private:
  /// <summary>
  ///
  /// </summary>
  class implementation;

  /// <summary>
  ///
  /// </summary>
  implementation *impl_;
};

typedef std::shared_ptr<ap_server> ap_server_ptr;
} // namespace aps
#endif // AP_SERVER_H
