//
// Created by shiontian on 11/13/2018.
//

#include "jni_ap_handler.h"

jni_ap_handler::jni_ap_handler() {}

void jni_ap_handler::on_thread_start() {
}

void jni_ap_handler::on_thread_stop() {
}

void jni_ap_handler::on_mirror_stream_started() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_started");
}

void jni_ap_handler::on_mirror_stream_data(const void *data) {
  __android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_mirror_stream_data");
}

void jni_ap_handler::on_mirror_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_stopped");
}

void jni_ap_handler::on_audio_set_volume(const float ratio,
                                          const float volume) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_volume");
}

void jni_ap_handler::on_audio_set_progress(const float ratio,
                                            const uint64_t start,
                                            const uint64_t current,
                                            const uint64_t end) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_progress");
}

void jni_ap_handler::on_audio_set_cover(const std::string format,
                                         const void *data,
                                         const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_cover");
}

void jni_ap_handler::on_audio_set_meta_data(const void *data,
                                             const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_meta_data");
}

void jni_ap_handler::on_audio_stream_started() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_started");
}

void jni_ap_handler::on_audio_stream_data(const void *data) {
  __android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_audio_stream_data");
}

void jni_ap_handler::on_audio_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_stopped");
}

void jni_ap_handler::on_video_play(const std::string &location,
                                    const float start_pos) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_play");
}

void jni_ap_handler::on_video_scrub(const float position) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_scrub");
}

void jni_ap_handler::on_video_rate(const float value) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_rate");
}

void jni_ap_handler::on_video_stop() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_stop");
}

void jni_ap_handler::on_acquire_playback_info(playback_info_t &playback_info) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_acquire_playback_info");
}