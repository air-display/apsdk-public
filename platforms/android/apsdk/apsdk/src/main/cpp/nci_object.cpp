/*
 *  File: nci_object.cpp
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
#include "nci_object.h"
// clang-format on

jclass nci_core::clz_ = 0;

jfieldID nci_core::field_nci_obj_ = 0;

void nci_core::initialize(JNIEnv *env) {
  clz_ = env->FindClass("com/sheentech/apsdk/NciObject");
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
  static jclass cls_NullPointerException = env->FindClass("java/lang/NullPointerException");
  // static jmethodID methodId =
  //     env->GetMethodID(cls_NullPointerException, "<init>", "()V");
  env->ThrowNew(cls_NullPointerException, "Null pointer");
}
