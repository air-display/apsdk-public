/*
 *  File: IAirPlayCastingHandler.h
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

#ifndef APS_SDK_AIRPLAY_VIDEO_HANDLER_H
#define APS_SDK_AIRPLAY_VIDEO_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayCastingHandler_cls[] = "com/sheentech/apsdk/IAirPlayCastingHandler";
class IAirPlayCastingHandler : public jni_meta_object<IAirPlayCastingHandler, IAirPlayCastingHandler_cls> {

public:
  IAirPlayCastingHandler(JNIEnv *env);

  ap_casting_session_handler_ptr get_casting_session_handler();

  void on_video_play(const uint64_t session_id, const std::string &location, const float start_pos);

  void on_video_scrub(const uint64_t session_id, const float position);

  void on_video_rate(const uint64_t session_id, const float value);

  void on_video_stop(const uint64_t session_id);

  void on_acquire_playback_info(const uint64_t session_id, playback_info_t &playback_info);

private:
  jni_ap_video_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_VIDEO_HANDLER_H
