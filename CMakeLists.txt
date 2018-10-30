cmake_minimum_required(VERSION 3.6)

project(aps)

# Include paths
include_directories(
    src
    third-party
    third-party/asio/include
)

# aps-demo
if ((NOT DEFINED BUILD_APS_DEMO) OR (BUILD_APS_DEMO STREQUAL "") 
    OR (${BUILD_APS_DEMO} MATCHES "(FALSE|false|0|OFF)"))
    option(BUILD_APS_DEMO "Build the aps demo" OFF)
elseif(${BUILD_APS_DEMO} MATCHES "(TRUE|true|1|null|ON)")
    option(BUILD_APS_DEMO "Build the aps demo" ON)
else()
    message(FATAL_ERROR "INVALID FLAG BUILD_APS_DEMO=" ${BUILD_APS_DEMO})
endif()
if (BUILD_APS_DEMO)
    add_subdirectory(demo)
endif()

# srouce 
add_subdirectory(src)

# third-party
add_subdirectory(third-party)