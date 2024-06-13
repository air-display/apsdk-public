/*
 *  File: nci_object.h
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public)
 *  Copyright (C) 2018-2024 Sheen Tian
 *
 *  apsdk is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Foundation,
 *  either version 3 of the License, or (at your option) any later version.
 *
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with Foobar.
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef APS_SDK_NCI_OBJECT_H
#define APS_SDK_NCI_OBJECT_H
// clang-format off
#include "aps-jni.h"
#include "jni_class_loader.h"
#include "jni_class_wrapper.h"
// clang-format on

class nci_core {
  static jclass clz_;
  static jfieldID field_nci_obj_;

public:
  static void initialize(JNIEnv *env);

  static jlong get_nciPtr(JNIEnv *env, jobject o);

  static void set_nciPtr(JNIEnv *env, jobject o, jlong p);

  static void throw_null_exception(JNIEnv *env);

private:
  nci_core() = delete;
};

template <typename T, const char *CLS> class nci_object : public jni_class_meta<CLS> {
public:
  static jobject new_jvmObject(JNIEnv *env) {
    return env->NewObject(jni_class_meta<CLS>::get_class(env), jni_class_meta<CLS>::get_constructor(env));
  }

  static T *attach(JNIEnv *env, jobject o) {
    T *p = new (std::nothrow) T(env);
    if (p) {
      p->jvm_obj_ = env->NewWeakGlobalRef(o);
      nci_core::set_nciPtr(env, p->jvm_obj_, (jlong)p);
    }
    return p;
  }

  static T *get(JNIEnv *env, jobject o) { return (T *)(void *)(nci_core::get_nciPtr(env, o)); }

  static void destroy(JNIEnv *env, jobject o) {
    T *p = T::get(env, o);
    if (0 == p) {
      return nci_core::throw_null_exception(env);
    }
    jobject ref = p->jvm_obj_;
    delete (T *)((void *)(p));
    if (ref) {
      env->DeleteWeakGlobalRef(ref);
    }
  }

protected:
  jobject jvm_obj_;
};

#define DEFINE_NCI_METHODS(x)                                                                                          \
  extern "C" JNIEXPORT void JNICALL Java_com_sheentech_apsdk_##x##_nciNew(JNIEnv *env, jobject thiz) {                 \
    x::attach(env, thiz);                                                                                              \
  }                                                                                                                    \
                                                                                                                       \
  extern "C" JNIEXPORT void JNICALL Java_com_sheentech_apsdk_##x##_nciDelete(JNIEnv *env, jobject thiz) {              \
    x::destroy(env, thiz);                                                                                             \
  }

#endif // APS_SDK_NCI_OBJECT_H
