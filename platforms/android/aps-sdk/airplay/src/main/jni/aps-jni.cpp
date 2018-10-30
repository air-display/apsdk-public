#include <string>
#include <android/log.h>
#include <ap_config.h>
#include <ap_server.h>
#include "aps-jni.h"

#include <dlfcn.h>

#define LOG_TAG "APS-JNI"

aps::ap_server* g_ap_server = 0;

bool Java_com_medialab_airplay_AirPlayServer_start(
    JNIEnv* env,
    jobject /* this */) {

    if (!g_ap_server)
        g_ap_server = new aps::ap_server(aps::ap_config::default_instance());

    g_ap_server->start();
    
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