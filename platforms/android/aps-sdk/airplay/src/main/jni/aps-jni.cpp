#include <string>
#include <android/log.h>
#include <ap_server.h>
#include "aps-jni.h"

#default LOG_TAG "APS-JNI"

JavaVM* __current_jvm___ = 0;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_write(
        ANDROID_LOG_DEBUG,
        LOG_TAG,
        "JNI_OnLoad");
    JNIEnv* env = NULL;
    jint result = JNI_ERR;
    __current_jvm___ = vm;

    if ((*vm)->GetEnv((void**) &env, JNI_VERSION_1_6) == JNI_OK) {
    }
    return result;
}

bool Java_com_medialab_airplay_AirPlayServer_start(
    JNIEnv* env,
    jobject /* this */) {

    std::string info = "AirPlay Server starts";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        LOG_TAG,
        info.c_str());
    return true;
}

void Java_com_medialab_airplay_AirPlayServer_stop(
    JNIEnv* env,
    jobject /* this */) {
    std::string info = "AirPlay Server stops";

    __android_log_write(
        ANDROID_LOG_DEBUG,
        LOG_TAG,
        info.c_str());
}