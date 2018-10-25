cmake_minimum_required(VERSION 3.6)

project(aps)

add_definitions(-DASIO_STANDALONE)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()

if (APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

include_directories(./third-party
    ./third-party/asio/include
)

file(GLOB_RECURSE SRC_AP
    src/aps.cpp
    src/ap_server.h
    src/ap_server.cpp
    src/ap_crypto.h
    src/ap_crypto.cpp
    src/ap_config.h
    src/ap_config.cpp
)
file(GLOB_RECURSE SRC_UTILS
    src/utils/logger.cpp
    src/utils/logger.h
    src/utils/packing.h
    src/utils/plist.cpp
    src/utils/plist.h
    src/utils/utils.cpp
    src/utils/utils.h
)
file(GLOB_RECURSE SRC_SERVICE
    src/service/ap_airplay_service.cpp
    src/service/ap_airplay_service.h
    src/service/ap_airplay_service_details.h
    src/service/ap_audio_stream_service.cpp
    src/service/ap_audio_stream_service.h
    src/service/ap_types.h
    src/service/ap_video_stream_service.cpp
    src/service/ap_video_stream_service.h
    src/service/ap_video_stream_service_details.h
)
file(GLOB_RECURSE SRC_NETWORK
    src/network/tcp_service.h
)
file(GLOB_RECURSE SRC_CRYPTO
    src/crypto/aes.c
    src/crypto/aes.h
    src/crypto/aes.hpp
)
file(GLOB_RECURSE SRC_THIRD
    src/crypto/aes.c
    src/crypto/aes.h
    src/crypto/aes.hpp
)

add_library(libaps STATIC
    ${SRC_AP}
    ${SRC_UTILS}
    ${SRC_SERVICE}
    ${SRC_NETWORK}
    ${SRC_CRYPTO}
)

source_group("ap" FILES ${SRC_AP})
source_group("utils" FILES ${SRC_UTILS})
source_group("service" FILES ${SRC_SERVICE})
source_group("network" FILES ${SRC_NETWORK})
source_group("crypto" FILES ${SRC_CRYPTO})

add_subdirectory(third-party/curve25519)
add_subdirectory(third-party/ed25519)
add_subdirectory(third-party/playfair)
add_subdirectory(demo)
