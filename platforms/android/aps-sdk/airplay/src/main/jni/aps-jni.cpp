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
    jobject thiz) {

    // Launch the mdnsd daemon process
    jclass thisClass = env->GetObjectClass(thiz);
    jmethodID acquireMdnsd = env->GetMethodID(
        thisClass, "acquireMdnsd","()V");
    env->CallVoidMethod(thiz, acquireMdnsd);

    if (!g_ap_server)
        g_ap_server = new aps::ap_server(aps::ap_config::default_instance());

    bool result = g_ap_server->start();
    
    std::string info = "AirPlay Server starts";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        LOG_TAG,
        info.c_str());
    return result;
}

void Java_com_medialab_airplay_AirPlayServer_stop(
    JNIEnv* env,
    jobject thiz) {

    // Stop the mdnsd daemon process
    jclass thisClass = env->GetObjectClass(thiz);
    jmethodID releaseMdnsd = env->GetMethodID(
        thisClass, "releaseMdnsd","()V");
    env->CallVoidMethod(thiz, releaseMdnsd);

    if (g_ap_server)
        g_ap_server->stop();

    std::string info = "AirPlay Server stops";
    __android_log_write(
        ANDROID_LOG_DEBUG,
        LOG_TAG,
        info.c_str());
}