//
// Created by shiontian on 11/12/2018.
//

#ifndef APS_SDK_AIRPLAY_HANDLER_H
#define APS_SDK_AIRPLAY_HANDLER_H
#include <aps-jni.h>
#include <jni_ap_handler.h>
#include <stdint.h>


using namespace aps;

class airplay_handler : public nci_object<airplay_handler> {
  static jclass clz_;
  static jmethodID mid_on_mirror_stream_start_;
  // static jmethodID mid_on_mirror_stream_data_;
  // static jmethodID mid_on_mirror_stream_stop_;
  // static jmethodID mid_on_audio_set_volume_;
  // static jmethodID mid_on_audio_set_progress_;
  // static jmethodID mid_on_audio_set_cover_;
  // static jmethodID mid_on_audio_set_meta_data_;
  // static jmethodID mid_on_audio_stream_stopped_;
  // static jmethodID mid_on_audio_stream_data_;
  // static jmethodID mid_on_video_play_;
  // static jmethodID mid_on_video_scrub_;
  // static jmethodID mid_on_video_rate_;
  // static jmethodID mid_on_video_stop_;
  // static jmethodID mid_on_acquire_playback_info_;

public:
  airplay_handler(jobject o);

  static void initialize(JavaVM *vm, JNIEnv *env);

  void set_javavm(JavaVM *vm);

  void set_jobject(jobject thiz);

  ap_handler_ptr get_ap_handler();

  void attach_thread();

  void detach_thread();

private:
  jobject thiz_;
  jni_ap_handler_ptr handler_;
};

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_medialab_airplay_AirPlayHandler_nciNew(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayHandler_nciDelete(JNIEnv *env, jobject thiz);
}
#endif // APS_SDK_AIRPLAY_HANDLER_H
