#pragma once
#include <asio.hpp>
#include <utils/packing.h>
#include <vector>

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
} // namespace details
} // namespace audio
} // namespace service
} // namespace aps