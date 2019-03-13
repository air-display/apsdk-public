#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>

#include <asio.hpp>

#include <utils/logger.h>
#include <utils/packing.h>

//#define DUMP_REQUEST_BODY 1

namespace aps {
namespace service {
namespace details {

// Headers used in AirPlay protocol
static const char *HEADER_CSEQ = "CSeq";
static const char *HEADER_SESSION = "Session";
static const char *HEADER_AUDIO_JACK_STATUS = "Audio-Jack-Status";
static const char *HEADER_APPLE_SESSION_ID = "X-Apple-Session-ID";

/// <summary>
///
/// </summary>
enum stream_type_e {
  audio = 96,
  video = 110,
};
typedef stream_type_e stream_type_t;

/// <summary>
///
/// </summary>
PACKED(struct pair_verify_header_s {
  uint8_t is_first_frame;
  uint8_t reserved0;
  uint8_t reserved1;
  uint8_t reserved2;
});
typedef pair_verify_header_s pair_verify_header_t;

/// <summary>
///
/// </summary>
PACKED(struct fp_header_s {
  uint8_t signature[4]; // Always 'FPLY'
  uint8_t major_version;
  uint8_t minor_version;
  uint8_t phase;
});
typedef fp_header_s fp_header_t;

static const char *ERROR_STATUS_RESPONSE =
    // clang-format off
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
    "<plist version=\"1.0\">\n"
    "<dict>\n"
    "\t<key>errorCode</key>\n"
    "\t<integer>0</integer>\n"
    "</dict>\n"
    "</plist>";
// clang-format on

} // namespace details
} // namespace service
} // namespace aps
