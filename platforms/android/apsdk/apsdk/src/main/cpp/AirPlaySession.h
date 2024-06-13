/*
 *  File: AirPlaySession.h
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

#ifndef APS_SDK_AIRPLAY_SESSION_H
#define APS_SDK_AIRPLAY_SESSION_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "IAirPlayMirroringHandler.h"
#include "IAirPlayCastingHandler.h"
// clang-format on

using namespace aps;

static const char AirPlaySession_cls[] = "com/sheentech/apsdk/AirPlaySession";
class AirPlaySession : public nci_object<AirPlaySession, AirPlaySession_cls> {
public:
  AirPlaySession(JNIEnv *env);

  void set_ap_session(const ap_session_ptr &p);

  uint64_t get_session_id();

  int get_session_type();

  void setMirroringHandler(IAirPlayMirroringHandler *handler);

  void setCastingHandler(IAirPlayCastingHandler *handler);

  void disconnect();

private:
  ap_session_weakptr session_;
};

#endif // APS_SDK_AIRPLAY_SESSION_H
