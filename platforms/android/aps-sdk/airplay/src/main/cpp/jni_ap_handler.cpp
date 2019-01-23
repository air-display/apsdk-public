//
// Created by shiontian on 11/13/2018.
//

// clang-format off
#include "nci_object.h"
#include "jni_ap_handler.h"
#include "IAirPlayHandler.h"
#include "IAirPlayMirrorHandler.h"
#include "IAirPlayVideoHandler.h"
// clang-format on

void attach_thread() {
  JavaVM *vm = getJavaVM();
  if (vm) {
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name = 0;
    args.group = 0;
    JNIEnv *env = 0;
    int status = vm->AttachCurrentThread(&env, &args);
    if (JNI_OK != status) {
      __android_log_write(ANDROID_LOG_ERROR, LOG_TAG,
                          "Failed to attach the thread");
    }
  }
}

void detach_thread() {
  JavaVM *vm = getJavaVM();
  if (vm) {
    vm->DetachCurrentThread();
  }
}

jni_ap_handler::jni_ap_handler(IAirPlayHandler *p) : proxy(p) {}

void jni_ap_handler::on_thread_start() { attach_thread(); }

jni_ap_handler::~jni_ap_handler() {
  if (proxy) {
    delete proxy;
  }
}

void jni_ap_handler::on_thread_stop() { detach_thread(); }

void jni_ap_handler::on_session_begin(
    aps::ap_session_ptr session) {
  if (proxy)
    proxy->on_session_begin(session);
}

void jni_ap_handler::on_session_end(const uint64_t session_id) {
  if (proxy)
    proxy->on_session_end(session_id);
}

jni_ap_mirror_handler::jni_ap_mirror_handler(IAirPlayMirrorHandler *p)
    : proxy(p) {}

void jni_ap_mirror_handler::on_thread_start() { attach_thread(); }

void jni_ap_mirror_handler::on_thread_stop() { detach_thread(); }

void jni_ap_mirror_handler::on_mirror_stream_started() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_started");
  if (proxy)
    proxy->on_mirror_stream_started();
}

void jni_ap_mirror_handler::on_mirror_stream_codec(
    const aps::sms_video_codec_packet_t *p) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_codec");
  if (proxy)
    proxy->on_mirror_stream_codec(p);
}

void jni_ap_mirror_handler::on_mirror_stream_data(
    const aps::sms_video_data_packet_t *p) {
  //__android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_mirror_stream_data");
  if (proxy)
    proxy->on_mirror_stream_data(p);
}

void jni_ap_mirror_handler::on_mirror_stream_heartbeat() {
  //__android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_mirror_stream_heartbeat");
  if (proxy)
    proxy->on_mirror_stream_heartbeat();
}

void jni_ap_mirror_handler::on_mirror_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_stopped");
  if (proxy)
    proxy->on_mirror_stream_stopped();
}

void jni_ap_mirror_handler::on_audio_set_volume(const float ratio,
                                                const float volume) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_volume");
  if (proxy)
    proxy->on_audio_set_volume(ratio, volume);
}

void jni_ap_mirror_handler::on_audio_set_progress(const float ratio,
                                                  const uint64_t start,
                                                  const uint64_t current,
                                                  const uint64_t end) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_progress");
  if (proxy)
    proxy->on_audio_set_progress(ratio, start, current, end);
}

void jni_ap_mirror_handler::on_audio_set_cover(const std::string format,
                                               const void *data,
                                               const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_cover");
  if (proxy)
    proxy->on_audio_set_cover(format, data, length);
}

void jni_ap_mirror_handler::on_audio_set_meta_data(const void *data,
                                                   const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_meta_data");
  if (proxy)
    proxy->on_audio_set_meta_data(data, length);
}

void jni_ap_mirror_handler::on_audio_stream_started(
    const aps::audio_data_format_t format) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_started");
  if (proxy)
    proxy->on_audio_stream_started(format);
}

void jni_ap_mirror_handler::on_audio_stream_data(
    const aps::rtp_audio_data_packet_t *p, const uint32_t payload_length) {
  //__android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_audio_stream_data");
  if (proxy)
    proxy->on_audio_stream_data(p, payload_length);
}

void jni_ap_mirror_handler::on_audio_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_stopped");
  if (proxy)
    proxy->on_audio_stream_stopped();
}

jni_ap_video_handler::jni_ap_video_handler(IAirPlayVideoHandler *p)
    : proxy(p) {}

void jni_ap_video_handler::on_thread_start() { attach_thread(); }

void jni_ap_video_handler::on_thread_stop() { detach_thread(); }

void jni_ap_video_handler::on_video_play(const uint64_t session_id,
                                         const std::string &location,
                                         const float start_pos) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_play");
  if (proxy)
    proxy->on_video_play(session_id, location, start_pos);
}

void jni_ap_video_handler::on_video_scrub(const uint64_t session_id,
                                          const float position) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_scrub");
  if (proxy)
    proxy->on_video_scrub(session_id, position);
}

void jni_ap_video_handler::on_video_rate(const uint64_t session_id,
                                         const float value) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_rate");
  if (proxy)
    proxy->on_video_rate(session_id, value);
}

void jni_ap_video_handler::on_video_stop(const uint64_t session_id) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_stop");
  if (proxy)
    proxy->on_video_stop(session_id);
}

void jni_ap_video_handler::on_acquire_playback_info(
    const uint64_t session_id, playback_info_t &playback_info) {
  __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "on_acquire_playback_info");
  if (proxy)
    proxy->on_acquire_playback_info(session_id, playback_info);
}