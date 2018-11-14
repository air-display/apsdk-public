//
// Created by shiontian on 11/11/2018.
//

#ifndef APS_SDK_AIRPLAY_SERVER_H
#define APS_SDK_AIRPLAY_SERVER_H
#include <airplay_config.h>
#include <airplay_handler.h>
#include <aps-jni.h>

using namespace aps;

class airplay_server : public nci_object<airplay_server> {
public:
  static void initialize(JavaVM *vm, JNIEnv *env);

  airplay_server();

  bool start();

  void stop();

  void setConfig();

  void setHandler(airplay_handler *handler);

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
Java_com_medialab_airplay_AirPlayServer_nciSetConfig(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL Java_com_medialab_airplay_AirPlayServer_nciSetHandler(
    JNIEnv *env, jobject thiz, jobject handler);
}

#endif // APS_SDK_AIRPLAY_SERVER_H
