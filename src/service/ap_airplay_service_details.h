/* 
 *  File: ap_airplay_service_details.h
 *  Project: apsdk
 *  Created: Oct 25, 2018
 *  Author: Sheen Tian
 *  
 *  This file is part of apsdk (https://github.com/air-display/apsdk-public) 
 *  Copyright (C) 2018-2024 Sheen Tian 
 *  
 *  apsdk is free software: you can redistribute it and/or modify it under the terms 
 *  of the GNU General Public License as published by the Free Software Foundation, 
 *  either version 3 of the License, or (at your option) any later version.
 *  
 *  apsdk is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  See the GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License along with Foobar. 
 *  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <array>
#include <map>
#include <string>
#include <vector>

#include <asio.hpp>

#include <utils/logger.h>
#include <utils/packing.h>


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
