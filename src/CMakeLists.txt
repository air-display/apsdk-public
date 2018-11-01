project(aps)

# Flags
set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED)
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Definitions
add_definitions(-DASIO_STANDALONE)
if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()

# Source files
set(SRC_AP
    aps.h
    aps.cpp
    ap_server.h
    ap_server.cpp
    ap_config.h
    ap_config.cpp
    ap_handler.h
)
source_group("ap" FILES ${SRC_AP})

set(SRC_CRYPTO
    crypto/ap_crypto.h
    crypto/ap_crypto.cpp
    crypto/aes.c
    crypto/aes.h
)
source_group("crypto" FILES ${SRC_CRYPTO})

set(SRC_MDNS
    mdns/net_service.h
    mdns/net_service.cpp
    mdns/net_service_impl.h
)

if (WIN32)
    list(APPEND SRC_MDNS
        mdns/dns_sd.h
        mdns/Windows/net_service_impl.cpp
    )
elseif (APPLE)
    list(APPEND SRC_MDNS
        mdns/macOS/net_service_impl.mm
    )
elseif (ANDROID)
    list(APPEND SRC_MDNS
        mdns/dns_sd.h
        mdns/Android/net_service_impl.cpp
    )
endif()
source_group("mdns" FILES ${SRC_MDNS})

set(SRC_NETWORK
    network/rtp_types.h
    network/tcp_service.h
    network/udp_service.h
)
source_group("network" FILES ${SRC_NETWORK})

set(SRC_SERVICE
    service/ap_airplay_service.cpp
    service/ap_airplay_service.h
    service/ap_airplay_service_details.h
    service/ap_audio_stream_service.cpp
    service/ap_audio_stream_service.h
    service/ap_audio_stream_service_details.h
    service/ap_video_stream_service.cpp
    service/ap_video_stream_service.h
    service/ap_video_stream_service_details.h
    service/ap_timing_sync_service.cpp
    service/ap_timing_sync_service.h
)
source_group("service" FILES ${SRC_SERVICE})

set(SRC_UTILS
    utils/logger.cpp
    utils/logger.h
    utils/packing.h
    utils/plist.cpp
    utils/plist.h
    utils/utils.cpp
    utils/utils.h
)
source_group("utils" FILES ${SRC_UTILS})

# Output
add_library(aps STATIC
    ${SRC_AP}
    ${SRC_CRYPTO}
    ${SRC_NETWORK}
    ${SRC_MDNS}
    ${SRC_SERVICE}
    ${SRC_UTILS}
)

target_link_libraries(aps PUBLIC
    curve25519
    ed25519
    playfair
)

# Dependencies
if (APPLE)
    find_library(FOUNDATION_FX Foundation)
    if (NOT FOUNDATION_FX)
        message(FATAL_ERROR "Foundation.Framework not found")
    endif()

    target_link_libraries(aps PUBLIC ${FOUNDATION_FX})
elseif (ANDROID)
    target_link_libraries(aps PUBLIC mdnssd)
endif()