//
// Created by shiontian on 11/11/2018.
//

// clang-format off
#include "nci_object.h"
// clang-format on

jclass nci_core::clz_ = 0;

jfieldID nci_core::field_nci_obj_ = 0;

void nci_core::initialize(JNIEnv *env) {
  clz_ = env->FindClass("com/virtable/airplay/NciObject");
  if (clz_) {
    clz_ = static_cast<jclass>(env->NewGlobalRef(clz_));
    field_nci_obj_ = env->GetFieldID(clz_, "nci_obj_", "J");
  }
}

jlong nci_core::get_nciPtr(JNIEnv *env, jobject o) {
  if (field_nci_obj_) {
    return env->GetLongField(o, field_nci_obj_);
  }
  return 0;
}

void nci_core::set_nciPtr(JNIEnv *env, jobject o, jlong p) {
  if (field_nci_obj_) {
    env->SetLongField(o, field_nci_obj_, p);
  }
}

void nci_core::throw_null_exception(JNIEnv *env) {
  static jclass cls_NullPointerException =
      env->FindClass("java/lang/NullPointerException");
  // static jmethodID methodId =
  //     env->GetMethodID(cls_NullPointerException, "<init>", "()V");
  env->ThrowNew(cls_NullPointerException, "Null pointer");
}
