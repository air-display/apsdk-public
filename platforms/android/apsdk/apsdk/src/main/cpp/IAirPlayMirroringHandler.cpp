/*
 *  File: IAirPlayMirroringHandler.cpp
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
#include "IAirPlayMirroringHandler.h"
// clang-format on

static uint64_t normalize_ntp_to_ms(uint64_t ntp) {
  const uint32_t EPOCH = 2208988800ULL;        // January 1970, in NTP seconds.
  const double NTP_SCALE_FRAC = 4294967296ULL; // NTP fractional unit.
  uint64_t milliseconds = (ntp >> 32) * 1000 - EPOCH;
  uint32_t fraction = (ntp & 0x0ffffffff) * 1000 / NTP_SCALE_FRAC;
  return (milliseconds + fraction);
}

IAirPlayMirroringHandler::IAirPlayMirroringHandler(JNIEnv *env)
    : jni_meta_object<IAirPlayMirroringHandler, IAirPlayMirroringHandler_cls>() {
  handler_ = std::make_shared<jni_ap_mirror_handler>(this);
}

ap_mirroring_session_handler_ptr IAirPlayMirroringHandler::get_mirroring_session_handler() { return handler_; }

void IAirPlayMirroringHandler::on_video_stream_started() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stream_started, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_started");
    }
  }
}

void IAirPlayMirroringHandler::on_video_stream_codec(const aps::sms_video_codec_packet_t *p) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stream_codec, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(p->payload_size);
      env->SetByteArrayRegion(byte_array, 0, p->payload_size, (jbyte *)(p->payload));
      env->CallVoidMethod(jvm_obj_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_codec");
    }
  }
}

void IAirPlayMirroringHandler::on_video_stream_data(const aps::sms_video_data_packet_t *p) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stream_data, "([BJ)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(p->payload_size);
      env->SetByteArrayRegion(byte_array, 0, p->payload_size, (jbyte *)(p->payload));
      // convert the value
      jlong timestamp = (jlong)(uint32_t)normalize_ntp_to_ms(p->timestamp);
      env->CallVoidMethod(jvm_obj_, mid, byte_array, timestamp);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_data");
    }
  }
}

void IAirPlayMirroringHandler::on_video_stream_heartbeat() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stream_heartbeat, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_heartbeat");
    }
  }
}

void IAirPlayMirroringHandler::on_video_stream_stopped() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_video_stream_stopped");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_set_volume(const float ratio, const float volume) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_volume, "(FF)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, ratio, volume);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_set_volume");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_set_progress(const float ratio, const uint64_t start, const uint64_t current,
                                                     const uint64_t end) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_progress, "(FJJJ)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, ratio, start, current, end);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_set_progress");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_set_cover(const std::string format, const void *data, const uint32_t length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_cover, "(Ljava/lang/String;[B)V");
    if (mid) {
      LocalJvmObject<String> image_format(String::fromUTF8(env, format.c_str()));
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(jvm_obj_, mid, image_format.get(), byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_set_cover");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_set_meta_data(const void *data, const uint32_t length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_meta_data, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(jvm_obj_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_set_meta_data");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_stream_started(const aps::audio_data_format_t format) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_started, "(I)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, format);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_stream_started");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                                    const uint32_t payload_length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_data, "([BJ)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(payload_length);
      env->SetByteArrayRegion(byte_array, 0, payload_length, (jbyte *)(p->payload));
      env->CallVoidMethod(jvm_obj_, mid, byte_array, (jlong)(p->timestamp));
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_stream_data");
    }
  }
}

void IAirPlayMirroringHandler::on_audio_stream_stopped() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, "Failed to get method id of on_audio_stream_stopped");
    }
  }
}
