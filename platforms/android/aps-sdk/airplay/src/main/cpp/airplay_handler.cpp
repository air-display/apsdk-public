//
// Created by shiontian on 11/12/2018.
//

#include "airplay_handler.h"
#include <nci_object.h>

jclass airplay_handler::clz_ = 0;

jmethodID airplay_handler::mid_on_mirror_stream_start_ = 0;

airplay_handler::airplay_handler(jobject o) : nci_object<airplay_handler>() {
  handler_ = std::make_shared<jni_ap_handler>();
}

void airplay_handler::initialize(JavaVM *vm, JNIEnv *env) {
  clz_ = env->FindClass("com/medialab/airplay/AirPlayHandler");
  if (clz_) {
    mid_on_mirror_stream_start_ =
        env->GetMethodID(clz_, "on_mirror_stream_start", "()V");
  }
}

void airplay_handler::set_javavm(JavaVM *vm) {}

void airplay_handler::set_jobject(jobject thiz) {}

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
    if (JNI_OK != 0) {
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

jlong Java_com_medialab_airplay_AirPlayHandler_nciNew(JNIEnv *env,
                                                      jobject thiz) {
  airplay_handler *p = airplay_handler::create(env, thiz);
  if (p) {
    JavaVM *vm = 0;
    env->GetJavaVM(&vm);
    p->set_javavm(vm);
    p->set_jobject(thiz);
  }
  return (jlong)p;
}

void Java_com_medialab_airplay_AirPlayHandler_nciDelete(JNIEnv *env,
                                                        jobject thiz) {
  airplay_handler::destroy(env, thiz);
}
