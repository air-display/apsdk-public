/*
 *  File: IAirPlayHandler.h
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

#ifndef APS_SDK_AIRPLAY_HANDLER_H
#define APS_SDK_AIRPLAY_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayHandler_cls[] = "com/sheentech/apsdk/IAirPlayHandler";
class IAirPlayHandler : public jni_meta_object<IAirPlayHandler, IAirPlayHandler_cls> {

public:
  IAirPlayHandler(JNIEnv *env);

  ap_handler_ptr get_ap_handler();

  void on_session_begin(ap_session_ptr session);

  void on_session_end(const uint64_t session_id);

private:
  jni_ap_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_HANDLER_H
