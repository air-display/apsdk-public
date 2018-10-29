#include <jni.h>

extern "C" {

jint JNI_OnLoad(JavaVM* vm, void* reserved);

JNIEXPORT bool JNICALL
Java_com_medialab_airplay_AirPlayServer_start(
        JNIEnv* env,
        jobject /* this */);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayServer_stop(
        JNIEnv* env,
        jobject /* this */);

}