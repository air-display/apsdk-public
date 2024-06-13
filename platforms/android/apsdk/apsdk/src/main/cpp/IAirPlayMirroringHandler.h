/*
 *  File: IAirPlayMirroringHandler.h
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

#ifndef APS_SDK_AIRPLAY_MIRROR_HANDLER_H
#define APS_SDK_AIRPLAY_MIRROR_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayMirroringHandler_cls[] = "com/sheentech/apsdk/IAirPlayMirroringHandler";
class IAirPlayMirroringHandler : public jni_meta_object<IAirPlayMirroringHandler, IAirPlayMirroringHandler_cls> {
public:
  IAirPlayMirroringHandler(JNIEnv *env);

  ap_mirroring_session_handler_ptr get_mirroring_session_handler();

  void on_video_stream_started();

  void on_video_stream_codec(const aps::sms_video_codec_packet_t *p);

  void on_video_stream_data(const aps::sms_video_data_packet_t *p);

  void on_video_stream_heartbeat();

  void on_video_stream_stopped();

  void on_audio_set_volume(const float ratio, const float volume);

  void on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current, const uint64_t end);

  void on_audio_set_cover(const std::string format, const void *data, const uint32_t length);

  void on_audio_set_meta_data(const void *data, const uint32_t length);

  void on_audio_stream_started(const aps::audio_data_format_t format);

  void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length);

  void on_audio_stream_stopped();

private:
  jni_ap_mirror_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_MIRROR_HANDLER_H
