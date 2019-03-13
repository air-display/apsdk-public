#pragma once
#include <vector>

#include <asio.hpp>

#include <utils/packing.h>

namespace aps {
namespace service {
namespace audio {
namespace details {
/// <summary>
///
/// </summary>
const int RTP_PACKET_MAX_LEN = 2048;

/// <summary>
///
/// </summary>
const int RTP_PACKET_MIN_LEN = 12;

enum audio_format_e {
  /// <summary>
  /// 96 AppleLossless
  /// 96 352 0 16 40 10 14 2 255 0 0 44100
  /// </summary>
  alac = 0x40000,

  /// <summary>
  /// 96 mpeg4-generic/44100/2
  /// 96 mode=AAC-main; constantDuration=1024
  /// </summary>
  aac_main = 0x400000,

  /// <summary>
  /// 96 mpeg4-generic/44100/2
  /// 96 mode=AAC-eld; constantDuration=480
  /// </summary>
  aac_eld = 0x1000000
};
typedef audio_format_e audio_format_t;

} // namespace details
} // namespace audio
} // namespace service
} // namespace aps
