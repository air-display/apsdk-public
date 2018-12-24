// clang-format off
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_class_loader.h"
// clang-format on

JavaVM *g_vm_ = 0;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
  g_vm_ = vm;
  JNIEnv *env;
  if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    return -1;
  }

  jni_class_loader::get().init(env);
  nci_core::initialize(env);

  return JNI_VERSION_1_6;
}

JavaVM *getJavaVM() { return g_vm_; }

JNIEnv *getJNIEnv() {
  JNIEnv *env = 0;
  JavaVM *vm = getJavaVM();
  if (vm) {
    jint r = vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (JNI_OK != r) {
      __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Failed to get JNIEnv %d",
                          r);
    }
  }
  return env;
}
