//
// Created by shiontian on 11/12/2018.
//

#include "airplay_handler.h"
#include <nci_object.h>

jclass airplay_handler::clz_ = 0;

jmethodID airplay_handler::mid_on_mirror_stream_started_ = 0;

jmethodID airplay_handler::mid_on_mirror_stream_data_ = 0;

jmethodID airplay_handler::mid_on_mirror_stream_stopped_ = 0;

jmethodID airplay_handler::mid_on_audio_set_volume_ = 0;

jmethodID airplay_handler::mid_on_audio_set_progress_ = 0;

jmethodID airplay_handler::mid_on_audio_set_cover_ = 0;

jmethodID airplay_handler::mid_on_audio_set_meta_data_ = 0;

jmethodID airplay_handler::mid_on_audio_stream_started_;

jmethodID airplay_handler::mid_on_audio_stream_data_ = 0;

jmethodID airplay_handler::mid_on_audio_stream_stopped_ = 0;

jmethodID airplay_handler::mid_on_video_play_ = 0;

jmethodID airplay_handler::mid_on_video_scrub_ = 0;

jmethodID airplay_handler::mid_on_video_rate_ = 0;

jmethodID airplay_handler::mid_on_video_stop_ = 0;

jmethodID airplay_handler::mid_on_acquire_playback_info_ = 0;

airplay_handler::airplay_handler(jobject o)
    : nci_object<airplay_handler>(), thiz_(o) {
  handler_ = std::make_shared<jni_ap_handler>(this);
}

#define GET_METHOD_ID(name, sig)                                               \
  mid_##name##_ = env->GetMethodID(clz_, #name, sig)

void airplay_handler::initialize(JavaVM *vm, JNIEnv *env) {
  clz_ = env->FindClass("com/medialab/airplay/IAirPlayHandler");
  if (clz_) {
    mid_on_mirror_stream_started_ =
        env->GetMethodID(clz_, "on_mirror_stream_started", "()V");
    GET_METHOD_ID(on_mirror_stream_data, "(Ljava/nio/ByteBuffer;)V");
    GET_METHOD_ID(on_mirror_stream_stopped, "()V");
    GET_METHOD_ID(on_audio_set_volume, "(FF)V");
    GET_METHOD_ID(on_audio_set_progress, "(FJJJ)V");
    GET_METHOD_ID(on_audio_set_cover,
                  "(Ljava/lang/String;Ljava/nio/ByteBuffer;J)V");
    GET_METHOD_ID(on_audio_set_meta_data, "(Ljava/nio/ByteBuffer;J)V");
    GET_METHOD_ID(on_audio_stream_started, "()V");
    GET_METHOD_ID(on_audio_stream_data, "(Ljava/nio/ByteBuffer;)V");
    GET_METHOD_ID(on_audio_stream_stopped, "()V");
    GET_METHOD_ID(on_video_play, "(Ljava/lang/String;F)V");
    GET_METHOD_ID(on_video_scrub, "(F)V");
    GET_METHOD_ID(on_video_rate, "(F)V");
    GET_METHOD_ID(on_video_stop, "()V");
    // GET_METHOD_ID(on_acquire_playback_info, "()V");
  }
}

JNIEnv *airplay_handler::get_JNIEnv() {
  JNIEnv *env = 0;
  JavaVM *vm = get_JavaVM();
  if (vm) {
    if (JNI_OK != vm->GetEnv((void **)&env, JNI_VERSION_1_6)) {
      __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "Failed to get JNIEnv");
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
    env->CallVoidMethod(thiz_, mid_on_mirror_stream_started_);
  }
}

void airplay_handler::on_mirror_stream_data(const void *data) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_mirror_stream_data_);
  }
}

void airplay_handler::on_mirror_stream_stopped() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_mirror_stream_stopped_);
  }
}

void airplay_handler::on_audio_set_volume(const float ratio,
                                          const float volume) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_set_volume_, ratio, volume);
  }
}

void airplay_handler::on_audio_set_progress(const float ratio,
                                            const uint64_t start,
                                            const uint64_t current,
                                            const uint64_t end) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_set_progress_, ratio, start,
                        current, end);
  }
}

void airplay_handler::on_audio_set_cover(const std::string format,
                                         const void *data,
                                         const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_set_cover_);
  }
}

void airplay_handler::on_audio_set_meta_data(const void *data,
                                             const uint32_t length) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_set_meta_data_);
  }
}

void airplay_handler::on_audio_stream_started() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_stream_started_);
  }
}

void airplay_handler::on_audio_stream_data(const void *data) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_stream_data_);
  }
}

void airplay_handler::on_audio_stream_stopped() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_audio_stream_stopped_);
  }
}

void airplay_handler::on_video_play(const std::string &location,
                                    const float start_pos) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    jstring l = env->NewStringUTF(location.c_str());
    env->CallVoidMethod(thiz_, mid_on_video_play_, l, start_pos);
  }
}

void airplay_handler::on_video_scrub(const float position) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_video_scrub_, position);
  }
}

void airplay_handler::on_video_rate(const float value) {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_video_rate_, value);
  }
}

void airplay_handler::on_video_stop() {
  JNIEnv *env = get_JNIEnv();
  if (env) {
    env->CallVoidMethod(thiz_, mid_on_video_stop_);
  }
}

// void airplay_handler::on_acquire_playback_info(playback_info_t
// &playback_info) {
// }

jlong Java_com_medialab_airplay_AirPlayHandler_nciNew(JNIEnv *env,
                                                      jobject thiz) {
  airplay_handler *p = airplay_handler::create(env, thiz);
  return (jlong)p;
}

void Java_com_medialab_airplay_AirPlayHandler_nciDelete(JNIEnv *env,
                                                        jobject thiz) {
  airplay_handler::destroy(env, thiz);
}
