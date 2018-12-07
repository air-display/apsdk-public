//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_AIRPLAY_SERVER_H
#define APS_SDK_AIRPLAY_SERVER_H
#include <airplay_handler.h>
#include <aps-jni.h>
#include "jni_class_wrapper.h"

using namespace aps;

class airplay_server : public nci_object<airplay_server> {
public:
  airplay_server(JNIEnv *env);

  bool start();

  void stop();

  void setConfig(ap_config_ptr config);

  void setHandler(airplay_handler *handler);

  short getServicePort();

private:
  ap_server_ptr server_;
};

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_medialab_airplay_AirPlayServer_nciNew(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_nciDelete(JNIEnv *env, jobject thiz);

JNIEXPORT jboolean JNICALL
Java_com_medialab_airplay_AirPlayServer_nciStart(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_nciStop(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_nciSetConfig(JNIEnv *env, jobject thiz, jobject config);

JNIEXPORT void JNICALL Java_com_medialab_airplay_AirPlayServer_nciSetHandler(
    JNIEnv *env, jobject thiz, jobject handler);

JNIEXPORT jshort JNICALL Java_com_medialab_airplay_AirPlayServer_nciGetServicePort(JNIEnv *env,
                                                                jobject thiz);
}

#endif // APS_SDK_AIRPLAY_SERVER_H
