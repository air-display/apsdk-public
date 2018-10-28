#include <jni.h>

extern "C" {

JNIEXPORT bool JNICALL
Java_com_medialab_airplay_AirPlayServer_initialize(
        JNIEnv* env,
        jobject /* this */);

JNIEXPORT bool JNICALL
Java_com_medialab_airplay_AirPlayServer_start(
        JNIEnv* env,
        jobject /* this */);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_stop(
        JNIEnv* env,
        jobject /* this */);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_uninitialize(
        JNIEnv* env,
        jobject /* this */);

}