/*
 *  File: jni_class_loader.h
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

#ifndef JNI_CLASS_LOADER_H
#define JNI_CLASS_LOADER_H
// clang-format off
#include "aps-jni.h"
// clang-format on

class jni_class_loader {
public:
  static jni_class_loader &get() {
    static jni_class_loader s_instance;
    return s_instance;
  }

  void init(JNIEnv *env) {
    jclass nci_object_clz = env->FindClass("com/sheentech/apsdk/NciObject");
    jclass class_clz = env->GetObjectClass(nci_object_clz);

    jclass class_loader_clz = env->FindClass("java/lang/ClassLoader");
    jmethodID mid_get_class_loader = env->GetMethodID(class_clz, "getClassLoader", "()Ljava/lang/ClassLoader;");
    class_loader_ = env->CallObjectMethod(nci_object_clz, mid_get_class_loader);
    class_loader_ = env->NewGlobalRef(class_loader_);

    mid_find_class_ = env->GetMethodID(class_loader_clz, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
  }

  jclass find_class(const char *name, JNIEnv *env) {
    return static_cast<jclass>(env->CallObjectMethod(class_loader_, mid_find_class_, env->NewStringUTF(name)));
  }

private:
  jobject class_loader_;
  jmethodID mid_find_class_;
};

#endif // JNI_CLASS_LOADER_H
