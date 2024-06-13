/* 
 *  File: logger.cpp
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

#if defined(ANDROID)
#include <android/log.h>
#endif
#include "logger.h"

log_config logger::log_config_;

void logger::init_logger(bool header, log_level level) {
  log_config_.headers = header;
  log_config_.level = level;
}

logger::logger() : opened_(false) {}

logger::logger(log_level level) : opened_(false) {
  msglevel_ = level;
  if (log_config_.headers) {
    operator<<("[" + get_lable(level) + "]");
  }
}

logger::~logger() {
  if (opened_) {
    oss_ << std::endl;
    std::string msg = oss_.str();
#if defined(ANDROID)
    __android_log_write(msglevel_, "APS", msg.c_str());
#else
    std::cout << msg;
#endif
  }
  opened_ = false;
}
