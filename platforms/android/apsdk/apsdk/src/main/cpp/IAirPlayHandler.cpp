/*
 *  File: IAirPlayHandler.cpp
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
#include "IAirPlayHandler.h"
#include "AirPlaySession.h"
// clang-format on

IAirPlayHandler::IAirPlayHandler(JNIEnv *env) : jni_meta_object<IAirPlayHandler, IAirPlayHandler_cls>() {
  (void)env;
  handler_ = std::make_shared<jni_ap_handler>(this);
}

ap_handler_ptr IAirPlayHandler::get_ap_handler() { return handler_; }

void IAirPlayHandler::on_session_begin(ap_session_ptr session) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_session_begin, "(Lcom/sheentech/apsdk/AirPlaySession;)V");
    if (mid) {
      // Create the airplay mirror session object
      jobject obj = AirPlaySession::new_jvmObject(env);
      AirPlaySession *s = AirPlaySession::attach(env, obj);
      s->set_ap_session(session);
      env->CallVoidMethod(jvm_obj_, mid, obj);
      env->DeleteLocalRef(obj);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_started");
    }
  }
}

void IAirPlayHandler::on_session_end(const uint64_t session_id) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_session_end, "(J)V");
    if (mid) {
      jlong sid = static_cast<jlong>(session_id);
      env->CallVoidMethod(jvm_obj_, mid, sid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_mirror_session_end");
    }
  }
}
