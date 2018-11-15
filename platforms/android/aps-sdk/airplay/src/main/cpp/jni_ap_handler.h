//
// Created by shiontian on 11/13/2018.
//

#ifndef APS_SDK_JNI_AP_HANDLER_H
#define APS_SDK_JNI_AP_HANDLER_H
#include <aps-jni.h>
#include <memory>

using namespace aps;

class airplay_handler;
class jni_ap_handler : public ap_handler {
public:
  jni_ap_handler(airplay_handler *p);

  virtual void on_thread_start() override;

  virtual void on_thread_stop() override;

  virtual void on_mirror_stream_started() override;

  virtual void on_mirror_stream_data(const void *data) override;

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

  virtual void on_audio_stream_started() override;

  virtual void on_audio_stream_data(const void *data) override;

  virtual void on_audio_stream_stopped() override;

  virtual void on_video_play(const std::string &location,
                             const float start_pos) override;

  virtual void on_video_scrub(const float position) override;

  virtual void on_video_rate(const float value) override;

  virtual void on_video_stop() override;

  virtual void
  on_acquire_playback_info(ap_handler::playback_info_t &playback_info) override;

private:
  airplay_handler *parent;
};

typedef std::shared_ptr<jni_ap_handler> jni_ap_handler_ptr;

#endif // APS_SDK_JNI_AP_HANDLER_H
