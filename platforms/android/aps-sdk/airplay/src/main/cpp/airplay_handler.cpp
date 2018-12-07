//
// Created by shiontian on 11/12/2018.
//

#include "airplay_handler.h"
#include "jni_class_wrapper.h"
#include <nci_object.h>

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

void airplay_handler::on_mirror_stream_started(const std::string &session) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_started, "()V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s);
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
                  "(Lcom/medialab/airplay/MirroringVideoCodec;)V");
    if (mid) {
      MirroringVideoCodec codec = MirroringVideoCodec::create(env);

      env->CallVoidMethod(obj_this_, mid, codec.get());
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
      env->CallVoidMethod(obj_this_, mid, byte_array, p->timestamp);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_mirror_stream_data");
    }
  }
}

void airplay_handler::on_mirror_stream_stopped(const std::string &session) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_mirror_stream_stopped, "()V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s);
    } else {
      __android_log_write(
          ANDROID_LOG_ERROR, LOG_TAG,
          "Failed to get method id of on_mirror_stream_stopped");
    }
  }
}

void airplay_handler::on_audio_set_volume(const std::string &session,
                                          const float ratio,
                                          const float volume) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_volume, "(FF)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s, ratio, volume);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_volume");
    }
  }
}

void airplay_handler::on_audio_set_progress(const std::string &session,
                                            const float ratio,
                                            const uint64_t start,
                                            const uint64_t current,
                                            const uint64_t end) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_progress, "(FJJJ)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s, ratio, start, current, end);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_progress");
    }
  }
}

void airplay_handler::on_audio_set_cover(const std::string &session,
                                         const std::string format,
                                         const void *data,
                                         const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_cover, "(Ljava/lang/String;[B)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      jstring image_format = env->NewStringUTF(format.c_str());
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(obj_this_, mid, s, image_format, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_cover");
    }
  }
}

void airplay_handler::on_audio_set_meta_data(const std::string &session,
                                             const void *data,
                                             const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_set_meta_data, "([B)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      jbyteArray byte_array = env->NewByteArray(length);
      env->SetByteArrayRegion(byte_array, 0, length, (jbyte *)data);
      env->CallVoidMethod(obj_this_, mid, s, byte_array);
      env->DeleteLocalRef(byte_array);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_audio_set_meta_data");
    }
  }
}

void airplay_handler::on_audio_stream_started(const std::string &session,
                                              const aps::audio_data_format_t format) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_started, "(I)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s, format);
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

void airplay_handler::on_audio_stream_stopped(const std::string &session) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_audio_stream_stopped, "()V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s);
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
    GET_METHOD_ID(on_video_play, "(Ljava/lang/String;F)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      jstring l = env->NewStringUTF(location.c_str());
      env->CallVoidMethod(obj_this_, mid, s, l, start_pos);
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
    GET_METHOD_ID(on_video_scrub, "(F)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s, position);
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
    GET_METHOD_ID(on_video_rate, "(F)V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s, value);
    } else {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to get method id of on_video_rate");
    }
  }
}

void airplay_handler::on_video_stop(const std::string &session) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    GET_METHOD_ID(on_video_stop, "()V");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      env->CallVoidMethod(obj_this_, mid, s);
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
    GET_METHOD_ID(get_playback_info, "()Lcom/medialab/airplay/PlaybackInfo;");
    if (mid) {
      jstring s = env->NewStringUTF(session.c_str());
      jobject object = env->CallObjectMethod(obj_this_, mid, s);
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
