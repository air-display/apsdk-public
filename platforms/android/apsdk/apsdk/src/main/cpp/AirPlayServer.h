/*
 *  File: AirPlayServer.h
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

#ifndef APS_SDK_AIRPLAY_SERVER_H
#define APS_SDK_AIRPLAY_SERVER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
#include "jni_class_wrapper.h"
#include "IAirPlayHandler.h"
// clang-format on

using namespace aps;

static const char AirPlayServer_cls[] = "com/sheentech/apsdk/AirPlayServer";
class AirPlayServer : public nci_object<AirPlayServer, AirPlayServer_cls> {
public:
  AirPlayServer(JNIEnv *env);

  bool start();

  void stop();

  void setConfig(const AirPlayConfig &config);

  void setHandler(IAirPlayHandler *handler);

  uint16_t getServicePort();

private:
  ap_server_ptr server_;
};

#endif // APS_SDK_AIRPLAY_SERVER_H
