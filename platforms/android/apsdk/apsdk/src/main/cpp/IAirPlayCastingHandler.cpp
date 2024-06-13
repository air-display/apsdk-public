/*
 *  File: IAirPlayCastingHandler.cpp
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

// clang-format off
#include <endian.h>
#include <ostream>
#include <sstream>
#include "jni_class_wrapper.h"
#include "IAirPlayCastingHandler.h"
// clang-format on

IAirPlayCastingHandler::IAirPlayCastingHandler(JNIEnv *env)
    : jni_meta_object<IAirPlayCastingHandler, IAirPlayCastingHandler_cls>() {
  (void)env;
  handler_ = std::make_shared<jni_ap_video_handler>(this);
}

ap_casting_session_handler_ptr IAirPlayCastingHandler::get_casting_session_handler() { return handler_; }

void IAirPlayCastingHandler::on_video_play(const uint64_t session_id, const std::string &location,
                                           const float start_pos) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_play, "(JLjava/lang/String;F)V");
    if (mid) {
      LocalJvmObject<String> l(String::fromUTF8(env, location.c_str()));
      env->CallVoidMethod(jvm_obj_, mid, session_id, l.get(), start_pos);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_play");
    }
  }
}

void IAirPlayCastingHandler::on_video_scrub(const uint64_t session_id, const float position) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_scrub, "(JF)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, session_id, position);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_scrub");
    }
  }
}

void IAirPlayCastingHandler::on_video_rate(const uint64_t session_id, const float value) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_rate, "(JF)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, session_id, value);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_rate");
    }
  }
}

void IAirPlayCastingHandler::on_video_stop(const uint64_t session_id) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stop, "(J)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, session_id);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stop");
    }
  }
}

void IAirPlayCastingHandler::on_acquire_playback_info(const uint64_t session_id, playback_info_t &playback_info) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(get_playback_info, "(J)Lcom/sheentech/apsdk/PlaybackInfo;");
    if (mid) {
      jobject object = env->CallObjectMethod(jvm_obj_, mid, session_id);
      if (object) {
        LocalJvmObject<PlaybackInfo> playbackInfo(PlaybackInfo::attach(env, object));
        playback_info.stallCount = (uint32_t)playbackInfo.stallCount();
        playback_info.duration = playbackInfo.duration();
        playback_info.position = playbackInfo.position();
        playback_info.rate = playbackInfo.rate();
        playback_info.readyToPlay = playbackInfo.readyToPlay();
        playback_info.playbackBufferEmpty = playbackInfo.playbackBufferEmpty();
        playback_info.playbackBufferFull = playbackInfo.playbackBufferFull();
        playback_info.playbackLikelyToKeepUp = playbackInfo.playbackLikelyToKeepUp();
      }
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_acquire_playback_info");
    }
  }
}
