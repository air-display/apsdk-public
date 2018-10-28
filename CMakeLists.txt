cmake_minimum_required(VERSION 3.6)

project(aps)

if ((NOT DEFINED BUILD_APS_DEMO) OR (BUILD_APS_DEMO STREQUAL "") 
    OR (${BUILD_APS_DEMO} MATCHES "(FALSE|false|0|OFF)"))
    option(BUILD_APS_DEMO "Build the aps demo" OFF)
elseif(${BUILD_APS_DEMO} MATCHES "(TRUE|true|1|null|ON)")
    option(BUILD_APS_DEMO "Build the aps demo" ON)
else()
    message(FATAL_ERROR "INVALID FLAG BUILD_APS_DEMO=" ${BUILD_APS_DEMO})
endif()

add_definitions(-DASIO_STANDALONE)

if (WIN32)
    add_definitions(-D_WIN32_WINNT=0x0501)
endif()

if (APPLE)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

include_directories(./third-party
    ./third-party/asio/include
)

file(GLOB_RECURSE SRC_AP
    src/aps.hpp
    src/aps.cpp
    src/ap_server.h
    src/ap_server.cpp
    src/ap_config.h
    src/ap_config.cpp
    src/ap_handler.h
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
    src/crypto/ap_crypto.h
    src/crypto/ap_crypto.cpp
    src/crypto/aes.c
    src/crypto/aes.h
    src/crypto/aes.hpp
)
file(GLOB_RECURSE SRC_THIRD
    src/crypto/aes.c
    src/crypto/aes.h
    src/crypto/aes.hpp
)

add_library(aps SHARED
    ${SRC_AP}
    ${SRC_UTILS}
    ${SRC_SERVICE}
    ${SRC_NETWORK}
    ${SRC_CRYPTO}
)

target_link_libraries(aps PRIVATE
    ed25519
    curve25519
    playfair
)

source_group("ap" FILES ${SRC_AP})
source_group("utils" FILES ${SRC_UTILS})
source_group("service" FILES ${SRC_SERVICE})
source_group("network" FILES ${SRC_NETWORK})
source_group("crypto" FILES ${SRC_CRYPTO})

add_subdirectory(third-party/curve25519)
add_subdirectory(third-party/ed25519)
add_subdirectory(third-party/playfair)

if (ANDROID)
    add_subdirectory(jni)
endif()

if (BUILD_APS_DEMO)
    add_subdirectory(demo)
endif()
