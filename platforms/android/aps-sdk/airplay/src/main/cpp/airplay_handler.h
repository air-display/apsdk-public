//
// Created by shiontian on 11/12/2018.
//

#ifndef APS_SDK_AIRPLAY_HANDLER_H
#define APS_SDK_AIRPLAY_HANDLER_H
#include <aps-jni.h>
#include <jni_ap_handler.h>
#include <mutex>
#include <stdint.h>

using namespace aps;

class airplay_handler : public nci_object<airplay_handler> {
  static jclass clz_this_;

public:
  airplay_handler(JNIEnv *env);

  JNIEnv *get_JNIEnv();

  ap_handler_ptr get_ap_handler();

  void attach_thread();

  void detach_thread();

  void on_mirror_stream_started();

  void on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p);

  void on_mirror_stream_data(const aps::sms_video_data_packet_t *p);

  void on_mirror_stream_stopped();

  void on_audio_set_volume(const float ratio, const float volume);

  void on_audio_set_progress(const float ratio, const uint64_t start,
                             const uint64_t current, const uint64_t end);

  void on_audio_set_cover(const std::string format, const void *data,
                          const uint32_t length);

  void on_audio_set_meta_data(const void *data, const uint32_t length);

  void on_audio_stream_started();

  void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                            const uint32_t payload_length);

  void on_audio_control_sync(const aps::rtp_control_sync_packet_t *p);

  void
  on_audio_control_retransmit(const aps::rtp_control_retransmit_packet_t *p);

  void on_audio_stream_stopped();

  void on_video_play(const std::string &location, const float start_pos);

  void on_video_scrub(const float position);

  void on_video_rate(const float value);

  void on_video_stop();

  void on_acquire_playback_info(ap_handler::playback_info_t &playback_info);

private:
  jni_ap_handler_ptr handler_;
};

extern "C" {
JNIEXPORT jlong JNICALL
Java_com_medialab_airplay_AirPlayHandler_nciNew(JNIEnv *env, jobject thiz);

JNIEXPORT void JNICALL
Java_com_medialab_airplay_AirPlayHandler_nciDelete(JNIEnv *env, jobject thiz);
}
#endif // APS_SDK_AIRPLAY_HANDLER_H
