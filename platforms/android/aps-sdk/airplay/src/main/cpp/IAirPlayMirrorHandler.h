//
// Created by shiontian on 11/12/2018.
//

#ifndef APS_SDK_AIRPLAY_MIRROR_HANDLER_H
#define APS_SDK_AIRPLAY_MIRROR_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayMirrorHandler_cls[] =
    "com/virtable/airplay/IAirPlayMirrorHandler";
class IAirPlayMirrorHandler
    : public jni_meta_object<IAirPlayMirrorHandler, IAirPlayMirrorHandler_cls> {
public:
  IAirPlayMirrorHandler(JNIEnv *env);

  ap_mirror_session_handler_ptr get_ap_mirror_session_handler();

  void on_mirror_stream_started();

  void on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p);

  void on_mirror_stream_data(const aps::sms_video_data_packet_t *p);

  void on_mirror_stream_heartbeat();

  void on_mirror_stream_stopped();

  void on_audio_set_volume(const float ratio, const float volume);

  void on_audio_set_progress(const float ratio, const uint64_t start,
                             const uint64_t current, const uint64_t end);

  void on_audio_set_cover(const std::string format, const void *data,
                          const uint32_t length);

  void on_audio_set_meta_data(const void *data, const uint32_t length);

  void on_audio_stream_started(const aps::audio_data_format_t format);

  void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                            const uint32_t payload_length);

  void on_audio_stream_stopped();

private:
  jni_ap_mirror_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_MIRROR_HANDLER_H