//
// Created by shiontian on 11/12/2018.
//

// clang-format off
#include <endian.h>
#include <ostream>
#include <sstream>
#include "jni_class_wrapper.h"
#include "IAirPlayMirrorHandler.h"
// clang-format on

IAirPlayMirrorHandler::IAirPlayMirrorHandler(JNIEnv *env)
    : jni_meta_object<IAirPlayMirrorHandler, IAirPlayMirrorHandler_cls>() {
  handler_ = std::make_shared<jni_ap_mirror_handler>(this);
}

ap_mirror_session_handler_ptr
IAirPlayMirrorHandler::get_ap_mirror_session_handler() {
  return handler_;
}

void IAirPlayMirrorHandler::on_mirror_stream_started() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_started, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_started");
    }
  }
}

void IAirPlayMirrorHandler::on_mirror_stream_codec(
    const aps::sms_video_codec_packet_t *p) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_codec, "([B)V");
    if (mid) {
      uint32_t sc = htonl(0x01);
      std::ostringstream oss;

      // Parse SPS
      uint8_t *cursor = (uint8_t *) p->start;
      for (int i = 0; i < p->sps_count; i++) {
        oss.write((char *) &sc, 0x4);
        uint16_t sps_length = *(uint16_t *) cursor;
        sps_length = ntohs(sps_length);
        cursor += sizeof(uint16_t);
        oss.write((char *) cursor, sps_length);
        cursor += sps_length;
      }

      // Parse PPS
      uint8_t pps_count = *cursor++;
      for (int i = 0; i < pps_count; i++) {
        oss.write((char *) &sc, 0x4);
        uint16_t pps_length = *(uint16_t *) cursor;
        pps_length = ntohs(pps_length);
        cursor += sizeof(uint16_t);
        oss.write((char *) cursor, pps_length);
        cursor += pps_length;
      }

      std::string buffer = oss.str();
      jbyteArray byte_array = env->NewByteArray(buffer.length());
      env->SetByteArrayRegion(byte_array, 0, buffer.length(),
                              (jbyte *) (buffer.c_str()));
      env->CallVoidMethod(jvm_obj_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_stream_codec");
    }
  }
}

void IAirPlayMirrorHandler::on_mirror_stream_data(
    const aps::sms_video_data_packet_t *p) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_data, "([BJ)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(p->payload_size);
      env->SetByteArrayRegion(byte_array, 0, p->payload_size,
                              (jbyte *) (p->payload));
      uint32_t sc = htonl(0x01);
      env->SetByteArrayRegion(byte_array, 0, 0x04, (jbyte *) &sc);
      env->CallVoidMethod(jvm_obj_, mid, byte_array, p->timestamp);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_stream_data");
    }
  }
}

void IAirPlayMirrorHandler::on_mirror_stream_stopped() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_stopped");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_set_volume(const float ratio,
                                                const float volume) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_volume, "(FF)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, ratio, volume);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_volume");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_set_progress(const float ratio,
                                                  const uint64_t start,
                                                  const uint64_t current,
                                                  const uint64_t end) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_progress, "(FJJJ)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, ratio, start, current, end);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_progress");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_set_cover(const std::string format,
                                               const void *data,
                                               const uint32_t length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_cover, "(Ljava/lang/String;[B)V");
    if (mid) {
      LocalJvmObject<String> image_format(
          String::fromUTF8(env, format.c_str()));
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *) data);
      env->CallVoidMethod(jvm_obj_, mid, image_format.get(), byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_cover");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_set_meta_data(const void *data,
                                                   const uint32_t length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_meta_data, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *) data);
      env->CallVoidMethod(jvm_obj_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_meta_data");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_stream_started(
    const aps::audio_data_format_t format) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_started, "(I)V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid, format);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_started");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_stream_data(
    const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length) {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_data, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(payload_length);
      env->SetByteArrayRegion(byte_array, 0, payload_length,
                              (jbyte *) (p->payload));
      env->CallVoidMethod(jvm_obj_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_data");
    }
  }
}

void IAirPlayMirrorHandler::on_audio_stream_stopped() {
  JNIEnv *env = getJNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(jvm_obj_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_stopped");
    }
  }
}