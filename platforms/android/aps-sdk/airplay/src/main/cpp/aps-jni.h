//
// Created by shiontian on 11/11/2018.
//

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
