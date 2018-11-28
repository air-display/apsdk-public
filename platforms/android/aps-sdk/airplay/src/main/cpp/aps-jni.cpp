#include "aps-jni.h"
#include <android/log.h>
#include <dlfcn.h>
#include <string>

#include <airplay_handler.h>
#include <airplay_server.h>
#include <jni_class_loader.h>
#include <nci_object.h>


#define LOG_TAG "APS-JNI"

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jni_class_loader::get().init(vm, env);

  nci_core::initialize(vm, env);
  return JNI_VERSION_1_6;
}