/*
 *  File: aps-jni.cpp
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

// clang-format off
#include <ap_server.h>
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
    jint r = vm->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (JNI_OK != r) {
      __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Failed to get JNIEnv %d", r);
    }
  }
  return env;
}
