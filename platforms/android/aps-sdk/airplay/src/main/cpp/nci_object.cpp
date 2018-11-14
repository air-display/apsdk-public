//
// Created by shiontian on 11/11/2018.
//

#include "nci_object.h"

JavaVM *nci_core::vm_ = 0;

jclass nci_core::clz_ = 0;

jfieldID nci_core::field_NicPtr_ = 0;

void nci_core::initialize(JavaVM *vm, JNIEnv *env) {
  vm_ = vm;
  clz_ = env->FindClass("com/medialab/nci/NciObject");
  if (clz_) {
    clz_ = static_cast<jclass>(env->NewGlobalRef(clz_));
    field_NicPtr_ = env->GetFieldID(clz_, "nciPtr", "J");
  }
}

JavaVM *nci_core::get_JavaVM() { return vm_; }

jlong nci_core::get_nciPtr(JNIEnv *env, jobject o) {
  if (field_NicPtr_) {
    return env->GetLongField(o, field_NicPtr_);
  }
  return 0;
}

void nci_core::throw_null_exception(JNIEnv *env) {
  static jclass cls_NullPointerException =
      env->FindClass("java/lang/NullPointerException");
  static jmethodID methodId =
      env->GetMethodID(cls_NullPointerException, "<init>", "()V");
  env->ThrowNew(cls_NullPointerException, "Null pointer");
}