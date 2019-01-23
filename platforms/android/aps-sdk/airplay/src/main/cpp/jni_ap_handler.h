//
// Created by shiontian on 11/13/2018.
//

#ifndef APS_SDK_JNI_AP_HANDLER_H
#define APS_SDK_JNI_AP_HANDLER_H
// clang-format off
#include <aps.h>
#include "aps-jni.h"
// clang-format on

using namespace aps;

class IAirPlayHandler;
class jni_ap_handler : public ap_handler {
public:
  jni_ap_handler(IAirPlayHandler *p);

  ~jni_ap_handler();

  virtual void on_thread_start() override;

  virtual void on_thread_stop() override;

  virtual void
  on_session_begin(aps::ap_session_ptr session) override;

  virtual void on_session_end(const uint64_t session_id) override;


private:
  IAirPlayHandler *proxy;
};
typedef std::shared_ptr<jni_ap_handler> jni_ap_handler_ptr;

class IAirPlayMirrorHandler;
class jni_ap_mirror_handler : public ap_mirror_session_handler {
public:
  jni_ap_mirror_handler(IAirPlayMirrorHandler *p);

  virtual void on_thread_start() override;

  virtual void on_thread_stop() override;

  virtual void on_mirror_stream_started() override;

  virtual void
  on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p) override;

  virtual void
  on_mirror_stream_data(const aps::sms_video_data_packet_t *p) override;

  virtual void
  on_mirror_stream_heartbeat() override;

  virtual void on_mirror_stream_stopped() override;

  virtual void on_audio_set_volume(const float ratio,
                                   const float volume) override;

  virtual void on_audio_set_progress(const float ratio, const uint64_t start,
                                     const uint64_t current,
                                     const uint64_t end) override;

  virtual void on_audio_set_cover(const std::string format, const void *data,
                                  const uint32_t length) override;

  virtual void on_audio_set_meta_data(const void *data,
                                      const uint32_t length) override;

  virtual void
  on_audio_stream_started(const aps::audio_data_format_t format) override;

  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                    const uint32_t payload_length) override;

  virtual void on_audio_stream_stopped() override;

private:
  IAirPlayMirrorHandler *proxy;
};
typedef std::shared_ptr<jni_ap_mirror_handler> jni_ap_mirror_handler_ptr;

class IAirPlayVideoHandler;
class jni_ap_video_handler : public ap_video_session_handler {
public:
  jni_ap_video_handler(IAirPlayVideoHandler *p);

  virtual void on_thread_start() override;

  virtual void on_thread_stop() override;

  virtual void on_video_play(const uint64_t session_id, const std::string &location,
                             const float start_pos) override;

  virtual void on_video_scrub(const uint64_t session_id,
                              const float position) override;

  virtual void on_video_rate(const uint64_t session_id, const float value) override;

  virtual void on_video_stop(const uint64_t session_id) override;

  virtual void
  on_acquire_playback_info(const uint64_t session_id,
                           playback_info_t &playback_info) override;

private:
  IAirPlayVideoHandler *proxy;
};
typedef std::shared_ptr<jni_ap_video_handler> jni_ap_video_handler_ptr;

#endif // APS_SDK_JNI_AP_HANDLER_H
