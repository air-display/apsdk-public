#include "aps-jni.h"
#include <android/log.h>
#include <dlfcn.h>
#include <string>

#include <nci_object.h>
#include <airplay_handler.h>
#include <airplay_server.h>


#define LOG_TAG "APS-JNI"

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  nci_core::initialize(vm, env);
  // airplay_server::initialize(vm, env);
  // airplay_handler::initialize(vm, env);

  return JNI_VERSION_1_6;
}