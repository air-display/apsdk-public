//
// Created by shiontian on 11/12/2018.
//

#ifndef APS_SDK_AIRPLAY_VIDEO_HANDLER_H
#define APS_SDK_AIRPLAY_VIDEO_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
#include "nci_object.h"
#include "jni_ap_handler.h"
// clang-format on

using namespace aps;

static const char IAirPlayVideoHandler_cls[] =
    "com/virtable/airplay/IAirPlayVideoHandler";
class IAirPlayVideoHandler
    : public jni_meta_object<IAirPlayVideoHandler, IAirPlayVideoHandler_cls> {

public:
  IAirPlayVideoHandler(JNIEnv *env);

  ap_video_session_handler_ptr get_ap_video_session_handler();

  void on_video_play(const uint64_t session_id, const std::string &location,
                     const float start_pos);

  void on_video_scrub(const uint64_t session_id, const float position);

  void on_video_rate(const uint64_t session_id, const float value);

  void on_video_stop(const uint64_t session_id);

  void on_acquire_playback_info(const uint64_t session_id,
                                playback_info_t &playback_info);

private:
  jni_ap_video_handler_ptr handler_;
};

#endif // APS_SDK_AIRPLAY_VIDEO_HANDLER_H
