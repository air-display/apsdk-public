#ifndef AP_SESSION_H
#define AP_SESSION_H
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <ap_types.h>

namespace aps {
/// <summary>
///
/// </summary>
class ap_mirror_session_handler {
public:
  virtual ~ap_mirror_session_handler(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_start(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_stop(){};

  // Mirroring
  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_mirror_stream_started() = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_mirror_stream_stopped() = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="p"></param>
  /// <remarks>
  /// THREAD_VIDEO_STREAM
  /// </remarks>
  virtual void
  on_mirror_stream_codec(const aps::sms_video_codec_packet_t *p) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="p"></param>
  /// <remarks>
  /// THREAD_VIDEO_STREAM
  /// </remarks>
  virtual void on_mirror_stream_data(const aps::sms_video_data_packet_t *p) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_VIDEO_STREAM
  /// </remarks>
  virtual void on_mirror_stream_heartbeat() = 0;

  // Audio
  /// <summary>
  ///
  /// </summary>
  /// <param name="ratio"></param>
  /// <param name="volume"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_audio_set_volume(const float ratio, const float volume) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="ratio"></param>
  /// <param name="start"></param>
  /// <param name="current"></param>
  /// <param name="end"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_audio_set_progress(const float ratio, const uint64_t start,
                                     const uint64_t current,
                                     const uint64_t end) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="format"></param>
  /// <param name="data"></param>
  /// <param name="length"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_audio_set_cover(const std::string format, const void *data,
                                  const uint32_t length) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="data"></param>
  /// <param name="length"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_audio_set_meta_data(const void *data,
                                      const uint32_t length) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void
  on_audio_stream_started(const aps::audio_data_format_t format) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="p"></param>
  /// <param name="payload_length"></param>
  /// <remarks>
  /// THREAD_AUDIO_STREAM
  /// </remarks>
  virtual void on_audio_stream_data(const aps::rtp_audio_data_packet_t *p,
                                    const uint32_t payload_length) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_audio_stream_stopped() = 0;
};

/// <summary>
///
/// </summary>
typedef std::shared_ptr<ap_mirror_session_handler>
    ap_mirror_session_handler_ptr;

/// <summary>
///
/// </summary>
typedef std::weak_ptr<ap_mirror_session_handler>
    ap_mirror_session_handler_weakptr;

/// <summary>
///
/// </summary>
struct time_range_s {
  /// <summary>
  ///
  /// </summary>
  double start;

  /// <summary>
  ///
  /// </summary>
  double duration;
};
typedef time_range_s time_range_t;
typedef std::vector<time_range_t> time_range_array;

/// <summary>
///
/// </summary>
struct playback_info_s {
  /// <summary>
  ///
  /// </summary>
  std::string uuid;

  /// <summary>
  ///
  /// </summary>
  uint32_t stallCount;

  /// <summary>
  ///
  /// </summary>
  double duration;

  /// <summary>
  ///
  /// </summary>
  float position;

  /// <summary>
  ///
  /// </summary>
  double rate;

  /// <summary>
  ///
  /// </summary>
  bool readyToPlay;

  /// <summary>
  ///
  /// </summary>
  bool playbackBufferEmpty;

  /// <summary>
  ///
  /// </summary>
  bool playbackBufferFull;

  /// <summary>
  ///
  /// </summary>
  bool playbackLikelyToKeepUp;

  /// <summary>
  ///
  /// </summary>
  time_range_array loadedTimeRanges;

  /// <summary>
  ///
  /// </summary>
  time_range_array seekableTimeRanges;

  playback_info_s() {
    stallCount = 0;
    duration = 0;
    position = 0;
    rate = 0;
    readyToPlay = false;
    playbackBufferEmpty = false;
    playbackBufferFull = false;
    playbackLikelyToKeepUp = false;
  }
};
typedef playback_info_s playback_info_t;

/// <summary>
///
/// <remarks>All methods will be called in the mirroring </remarks>
/// </summary>
class ap_video_session_handler {
public:
  virtual ~ap_video_session_handler(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_start(){};

  /// <summary>
  ///
  /// </summary>
  virtual void on_thread_stop(){};

  /// <summary>
  ///
  /// </summary>
  /// <param name="location"></param>
  /// <param name="start_pos"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_play(const uint64_t session_id,
                             const std::string &location,
                             const float start_pos) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="position"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_scrub(const uint64_t session_id,
                              const float position) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="value"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_rate(const uint64_t session_id, const float value) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_stop(const uint64_t session_id) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="playback_info"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_acquire_playback_info(const uint64_t session_id,
                                        playback_info_t &playback_info) = 0;
};

/// <summary>
///
/// </summary>
typedef std::shared_ptr<ap_video_session_handler> ap_video_session_handler_ptr;

/// <summary>
///
/// </summary>
typedef std::weak_ptr<ap_video_session_handler>
    ap_video_session_handler_weakptr;

enum session_type_e {
  mirror_session = 0,
  video_session = 1,

  unknown_session = (uint32_t)-1
};
typedef session_type_e session_type_t;

class ap_session {
public:
  virtual ~ap_session(){};

  virtual uint64_t get_session_id() = 0;

  virtual uint32_t get_session_type() = 0;

  virtual void disconnect() = 0;

  virtual void set_mirror_handler(ap_mirror_session_handler_ptr handler) = 0;

  virtual void set_video_handler(ap_video_session_handler_ptr handler) = 0;
};

typedef std::shared_ptr<ap_session> ap_session_ptr;
typedef std::weak_ptr<ap_session> ap_session_weakptr;

} // namespace aps

#endif // AP_SESSION_H
