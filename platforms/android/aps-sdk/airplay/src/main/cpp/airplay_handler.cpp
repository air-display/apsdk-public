//
// Created by shiontian on 11/12/2018.
//

#include "airplay_handler.h"
#include "jni_class_wrapper.h"
#include <nci_object.h>
#include <endian.h>
#include <ostream>
#include <sstream>

jclass airplay_handler::clz_this_ = 0;

#define GET_METHOD_ID(name, sig)                                               \
  static jmethodID mid = env->GetMethodID(clz_this_, #name, sig);              \
  if (!mid)                                                                    \
    mid = env->GetMethodID(clz_this_, #name, sig);                             \
  ;

airplay_handler::airplay_handler(JNIEnv *env) : nci_object<airplay_handler>() {
  if (!airplay_handler::clz_this_) {
    jclass clz = jni_class_loader::get().find_class(
        "com/medialab/airplay/IAirPlayHandler", env);
    if (clz)
      airplay_handler::clz_this_ = (jclass)env->NewGlobalRef(clz);
    else
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to find class: com/medialab/airplay/IAirPlayHandler");
  }
  handler_ = std::make_shared<jni_ap_handler>(this);
}

JNIEnv *airplay_handler::get_JNIEnv() {
  JNIEnv *env = 0;
  JavaVM *vm = get_JavaVM();
  if (vm) {
    jint r = vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (JNI_OK != r) {
      __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Failed to get JNIEnv %d",
                          r);
    }
  }
  return env;
}

ap_handler_ptr airplay_handler::get_ap_handler() { return handler_; }

void airplay_handler::attach_thread() {
  JavaVM *vm = nci_object::get_JavaVM();
  if (vm) {
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name = 0;
    args.group = 0;
    JNIEnv *env = 0;
    int status = vm->AttachCurrentThread(&env, &args);
    if (JNI_OK != status) {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to attach the thread");
    }
  }
}

void airplay_handler::detach_thread() {
  JavaVM *vm = nci_object::get_JavaVM();
  if (vm) {
    vm->DetachCurrentThread();
  }
}

void airplay_handler::on_mirror_stream_started() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_started, "()V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_started");
    }
  }
}

void airplay_handler::on_mirror_stream_codec(
    const aps::sms_video_codec_packet_t *p) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_codec,
                  "([B)V");
    if (mid) {
      uint32_t sc = htonl(0x01);
      std::ostringstream oss;

      // Parse SPS
      uint8_t *cursor = (uint8_t *)p->start;
      for (int i = 0; i < p->sps_count; i++) {
        oss.write((char *)&sc, 0x4);
        uint16_t sps_length = *(uint16_t *)cursor;
        sps_length = ntohs(sps_length);
        oss.write((char *)cursor, sps_length);
        cursor += sizeof(uint16_t) + sps_length;
      }

      // Parse PPS
      uint8_t pps_count = *cursor++;
      for (int i = 0; i < pps_count; i++) {
        oss.write((char *)&sc, 0x4);
        uint16_t pps_length = *(uint16_t *)cursor;
        pps_length = ntohs(pps_length);
        oss.write((char *)cursor, pps_length);
        cursor += sizeof(uint16_t) + pps_length;
      }

      std::string buffer = oss.str();
      jbyteArray byte_array = env->NewByteArray(buffer.length());
      env->SetByteArrayRegion(byte_array, 0, buffer.length(),
                              (jbyte *)(buffer.c_str()));
      env->CallVoidMethod(obj_this_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_stream_codec");
    }
  }
}

void airplay_handler::on_mirror_stream_data(
    const aps::sms_video_data_packet_t *p) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_data, "([BJ)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(p->payload_size);
      env->SetByteArrayRegion(byte_array, 0, p->payload_size,
                              (jbyte *)(p->payload));
      uint32_t sc = htonl(0x01);
      env->SetByteArrayRegion(byte_array, 0, 0x04, (jbyte *)&sc);
      env->CallVoidMethod(obj_this_, mid, byte_array, p->timestamp);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_stream_data");
    }
  }
}

void airplay_handler::on_mirror_stream_stopped() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_stopped");
    }
  }
}

void airplay_handler::on_audio_set_volume(const float ratio,
                                          const float volume) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_volume, "(FF)V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid, ratio, volume);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_volume");
    }
  }
}

void airplay_handler::on_audio_set_progress(const float ratio,
                                            const uint64_t start,
                                            const uint64_t current,
                                            const uint64_t end) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_progress, "(FJJJ)V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid, ratio, start, current, end);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_progress");
    }
  }
}

void airplay_handler::on_audio_set_cover(const std::string format,
                                         const void *data,
                                         const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_cover, "(Ljava/lang/String;[B)V");
    if (mid) {
      String image_format = String::fromUTF8(env, format.c_str());
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(obj_this_, mid, image_format.get(), byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_cover");
    }
  }
}

void airplay_handler::on_audio_set_meta_data(const void *data,
                                             const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_meta_data, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(obj_this_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_meta_data");
    }
  }
}

void airplay_handler::on_audio_stream_started(const aps::audio_data_format_t format) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_started, "(I)V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid, format);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_started");
    }
  }
}

void airplay_handler::on_audio_stream_data(
        const aps::rtp_audio_data_packet_t *p,
        const uint32_t payload_length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_data, "([B)V");
    if (mid) {
      jbyteArray byte_array = env->NewByteArray(payload_length);
      env->SetByteArrayRegion(byte_array, 0, payload_length,
                              (jbyte *)(p->payload));
      env->CallVoidMethod(obj_this_, mid, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_data");
    }
  }
}

// void airplay_handler::on_audio_control_sync(
//    const aps::rtp_control_sync_packet_t *p) {
//  JNIEnv *env = get_JNIEnv();
//  if (env) {
//    GET_METHOD_ID(on_audio_control_sync,
//                  "(Lcom/medialab/airplay/AudioControlSync;)V");
//    if (mid) {
//      AudioControlSync sync = AudioControlSync::create(env);
//      sync.sequence(p->sequence);
//      sync.timestamp(p->timestamp);
//      sync.currentNTPTme(p->current_ntp_time);
//      sync.nextPacketTime(p->next_packet_time);
//      env->CallVoidMethod(obj_this_, mid, sync.get());
//    } else {
//      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
//                          "Failed to get method id of on_audio_stream_data");
//    }
//  }
//}
//
// void airplay_handler::on_audio_control_retransmit(
//    const aps::rtp_control_retransmit_packet_t *p) {
//  JNIEnv *env = get_JNIEnv();
//  if (env) {
//    GET_METHOD_ID(on_audio_control_retransmit,
//                  "(Lcom/medialab/airplay/AudioControlRetransmit;)V");
//    if (mid) {
//      AudioControlRetransmit retransmit = AudioControlRetransmit::create(env);
//      retransmit.sequence(p->sequence);
//      retransmit.timestamp(p->timestamp);
//      retransmit.lostPacketStart(p->lost_packet_start);
//      retransmit.lostPacketCount(p->lost_packet_count);
//      env->CallVoidMethod(obj_this_, mid, retransmit.get());
//    } else {
//      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
//                          "Failed to get method id of on_audio_stream_data");
//    }
//  }
//}

void airplay_handler::on_audio_stream_stopped() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_stopped, "()V");
    if (mid) {
      env->CallVoidMethod(obj_this_, mid);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_stream_stopped");
    }
  }
}

void airplay_handler::on_video_play(const std::string &session,
                                    const std::string &location,
                                    const float start_pos) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_play, "(Ljava/lang/String;Ljava/lang/String;F)V");
    if (mid) {
      String s = String::fromUTF8(env, session.c_str());
      String l = String::fromUTF8(env, location.c_str());
      env->CallVoidMethod(obj_this_, mid, s.get(), l.get(), start_pos);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_video_play");
    }
  }
}

void airplay_handler::on_video_scrub(const std::string &session,
                                     const float position) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_scrub, "(Ljava/lang/String;F)V");
    if (mid) {
      String s = String::fromUTF8(env, session.c_str());
      env->CallVoidMethod(obj_this_, mid, s.get(), position);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_video_scrub");
    }
  }
}

void airplay_handler::on_video_rate(const std::string &session,
                                    const float value) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_rate, "(Ljava/lang/String;F)V");
    if (mid) {
      String s = String::fromUTF8(env, session.c_str());
      env->CallVoidMethod(obj_this_, mid, s.get(), value);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_video_rate");
    }
  }
}

void airplay_handler::on_video_stop(const std::string &session) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stop, "(Ljava/lang/String;)V");
    if (mid) {
      String s = String::fromUTF8(env, session.c_str());
      env->CallVoidMethod(obj_this_, mid, s.get());
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_video_stop");
    }
  }
}

void airplay_handler::on_acquire_playback_info(const std::string &session,
                                               ap_handler::playback_info_t &playback_info) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(get_playback_info, "(Ljava/lang/String;)Lcom/medialab/airplay/PlaybackInfo;");
    if (mid) {
      String s = String::fromUTF8(env, session.c_str());
      jobject object = env->CallObjectMethod(obj_this_, mid, s.get());
      if (object) {
        PlaybackInfo playbackInfo = PlaybackInfo::attach(env, object);
        playback_info.stallCount = (uint32_t)playbackInfo.stallCount();
        playback_info.duration = playbackInfo.duration();
        playback_info.position = playbackInfo.position();
        playback_info.rate = playbackInfo.rate();
        playback_info.readyToPlay = playbackInfo.readyToPlay();
        playback_info.playbackBufferEmpty = playbackInfo.playbackBufferEmpty();
        playback_info.playbackBufferFull = playbackInfo.playbackBufferFull();
        playback_info.playbackLikelyToKeepUp =
            playbackInfo.playbackLikelyToKeepUp();
      }
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_acquire_playback_info");
    }
  }
}

jlong Java_com_medialab_airplay_AirPlayHandler_nciNew(JNIEnv *env,
                                                      jobject thiz) {
  airplay_handler *p = airplay_handler::create(env, thiz);
  return reinterpret_cast<jlong>(p);
}

void Java_com_medialab_airplay_AirPlayHandler_nciDelete(JNIEnv *env,
                                                        jobject thiz) {
  airplay_handler::destroy(env, thiz);
}
