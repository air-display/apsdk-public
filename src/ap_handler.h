#ifndef AP_HANDLER_H
#define AP_HANDLER_H
#pragma once
#include <ap_types.h>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

namespace aps {
/// <summary>
///
/// </summary>
class ap_handler {
public:
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
  /// </summary>
  virtual ~ap_handler(){};

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

  // Video
  /// <summary>
  ///
  /// </summary>
  /// <param name="location"></param>
  /// <param name="start_pos"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_play(const std::string &session,
                             const std::string &location,
                             const float start_pos) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="position"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_scrub(const std::string &session,
                              const float position) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="value"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_rate(const std::string &session, const float value) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_video_stop(const std::string &session) = 0;

  /// <summary>
  ///
  /// </summary>
  /// <param name="playback_info"></param>
  /// <remarks>
  /// THREAD_AIRPLAY
  /// </remarks>
  virtual void on_acquire_playback_info(const std::string &session,
                                        playback_info_t &playback_info) = 0;
};

/// <summary>
///
/// </summary>
typedef std::shared_ptr<ap_handler> ap_handler_ptr;
} // namespace aps
#endif // AP_HANDLER_H
