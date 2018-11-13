//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_APS_JNI_H
#define APS_SDK_APS_JNI_H
#include <jni.h>
#include <android/log.h>
#include <ap_config.h>
#include <ap_handler.h>
#include <ap_server.h>
#include <nci_object.h>

#define LOG_TAG "APS-JNI"

extern "C" {
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved);
}

#endif // APS_SDK_APS_JNI_H
