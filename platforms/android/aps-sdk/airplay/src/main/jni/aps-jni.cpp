#include <string>
#include <android/log.h>
#include <ap_server.h>
#include "aps-jni.h"

bool Java_com_medialab_airplay_AirPlayServer_initialize(
    JNIEnv* env,
    jobject /* this */) {
    std::string info = "AirPlay Server initializes";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        "SHEEN",
        info.c_str());

    aps::ap_server* server = new aps::ap_server();
    server->initialize();
    server->start();

    return true;
}

bool Java_com_medialab_airplay_AirPlayServer_start(
    JNIEnv* env,
    jobject /* this */) {
    std::string info = "AirPlay Server starts";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        "SHEEN",
        info.c_str());
    return true;
}

void Java_com_medialab_airplay_AirPlayServer_stop(
    JNIEnv* env,
    jobject /* this */) {
    std::string info = "AirPlay Server stops";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        "SHEEN",
        info.c_str());
}

void Java_com_medialab_airplay_AirPlayServer_uninitialize(
    JNIEnv* env,
    jobject /* this */) {
    std::string info = "AirPlay Server uninitializes";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        "SHEEN",
        info.c_str());
}
