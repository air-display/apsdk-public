//
// Created by shiontian on 11/13/2018.
//

#include "jni_ap_handler.h"
#include <airplay_handler.h>

jni_ap_handler::jni_ap_handler(airplay_handler *p) : parent(p) {}

void jni_ap_handler::on_thread_start() {
  if (parent)
    parent->attach_thread();
}

void jni_ap_handler::on_thread_stop() {
  if (parent)
    parent->detach_thread();
}

void jni_ap_handler::on_mirror_stream_started() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_started");
  if (parent)
    parent->on_mirror_stream_started();
}

void jni_ap_handler::on_mirror_stream_codec(
    const aps::sms_video_codec_packet_t *p) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_codec");
  if (parent)
    parent->on_mirror_stream_codec(p);
}

void jni_ap_handler::on_mirror_stream_data(
    const aps::sms_video_data_packet_t *p) {
  __android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_mirror_stream_data");
  if (parent)
    parent->on_mirror_stream_data(p);
}

void jni_ap_handler::on_mirror_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_mirror_stream_stopped");
  if (parent)
    parent->on_mirror_stream_stopped();
}

void jni_ap_handler::on_audio_set_volume(const float ratio,
                                         const float volume) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_volume");
  if (parent)
    parent->on_audio_set_volume(ratio, volume);
}

void jni_ap_handler::on_audio_set_progress(const float ratio,
                                           const uint64_t start,
                                           const uint64_t current,
                                           const uint64_t end) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_progress");
  if (parent)
    parent->on_audio_set_progress(ratio, start, current, end);
}

void jni_ap_handler::on_audio_set_cover(const std::string format,
                                        const void *data,
                                        const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_cover");
  if (parent)
    parent->on_audio_set_cover(format, data, length);
}

void jni_ap_handler::on_audio_set_meta_data(const void *data,
                                            const uint32_t length) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_set_meta_data");
  if (parent)
    parent->on_audio_set_meta_data(data, length);
}

void jni_ap_handler::on_audio_stream_started() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_started");
  if (parent)
    parent->on_audio_stream_started();
}

void jni_ap_handler::on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                          const uint32_t payload_length) {
  __android_log_write(ANDROID_LOG_VERBOSE, LOG_TAG, "on_audio_stream_data");
  if (parent)
    parent->on_audio_stream_data(p, payload_length);
}

void jni_ap_handler::on_audio_control_sync(
    const aps::rtp_control_sync_packet_t *p) {
  __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "on_audio_control_sync");
  if (parent)
    parent->on_audio_control_sync(p);
}

void jni_ap_handler::on_audio_control_retransmit(
    const aps::rtp_control_retransmit_packet_t *p) {
  __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, "on_audio_control_retransmit");
  if (parent)
    parent->on_audio_control_retransmit(p);
}

void jni_ap_handler::on_audio_stream_stopped() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_audio_stream_stopped");
  if (parent)
    parent->on_audio_stream_stopped();
}

void jni_ap_handler::on_video_play(const std::string &location,
                                   const float start_pos) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_play");
  if (parent)
    parent->on_video_play(location, start_pos);
}

void jni_ap_handler::on_video_scrub(const float position) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_scrub");
  if (parent)
    parent->on_video_scrub(position);
}

void jni_ap_handler::on_video_rate(const float value) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_rate");
  if (parent)
    parent->on_video_rate(value);
}

void jni_ap_handler::on_video_stop() {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_video_stop");
  if (parent)
    parent->on_video_stop();
}

void jni_ap_handler::on_acquire_playback_info(
    ap_handler::playback_info_t &playback_info) {
  __android_log_write(ANDROID_LOG_INFO, LOG_TAG, "on_acquire_playback_info");
  if (parent)
    parent->on_acquire_playback_info(playback_info);
}