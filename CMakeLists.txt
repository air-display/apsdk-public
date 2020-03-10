cmake_minimum_required(VERSION 3.4.1)
message(STATUS "====================================")
project(aps)

message(STATUS "Current build target system:" ${CMAKE_SYSTEM_NAME})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
endif()

# Flags
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (WINDOWS)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
elseif (MACOSX)
    
elseif (ANDROID)

elseif (LINUX)

endif()

# Definitions
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_NO_TYPEID)
if (WINDOWS)
    add_definitions(-D_WIN32_WINNT=0x0600) # Windows Vista +
    add_definitions(-DINCL_EXTRA_HTON_FUNCTIONS)
elseif (MACOS)
    
elseif (ANDROID)

elseif (LINUX)

endif()

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