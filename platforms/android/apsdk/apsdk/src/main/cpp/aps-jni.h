/*
 *  File: aps-jni.h
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

#ifndef APS_SDK_APS_JNI_H
#define APS_SDK_APS_JNI_H
// clang-format off
#include <jni.h>
#include <android/log.h>
#include <stdint.h>
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <map>
// clang-format on

#define LOG_TAG "APS-JNI"

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);
}

JavaVM *getJavaVM();

JNIEnv *getJNIEnv();

#endif // APS_SDK_APS_JNI_H
