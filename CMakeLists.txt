cmake_minimum_required(VERSION 3.18)
project(apsdk)

option(BUILD_APS_DEMO "Build the demo project" OFF)

message(STATUS "++++++++++ Current build target system:" ${CMAKE_SYSTEM_NAME})
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(ANDROID TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(MACOSX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(IOS TRUE)
    if(${CMAKE_VERSION} VERSION_LESS "3.14.0") 
        message(FATAL_ERROR "CMake Version is too low to build for iOS platform, 3.14+ is required.")
    endif()
endif()

# Flags
# C standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)

# Hide symbols
if (NOT APPLE)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_C_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
endif()

# Enable project folder
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WINDOWS)

elseif (LINUX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
elseif (ANDROID)

elseif (MACOSX)

elseif (IOS)

endif()

# Definitions
add_definitions(-DASIO_STANDALONE)
if (WINDOWS)
    add_definitions(-D_WIN32_WINNT=0x0600) # Windows Vista +
    add_definitions(-DINCL_EXTRA_HTON_FUNCTIONS)
elseif (LINUX)
    
elseif (ANDROID)

elseif (MACOS)

elseif (IOS)

endif()

set(AP_PLATFORM_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/platforms)

# Include paths
include_directories(
    include
    third-party
    third-party/asio/include
)

# srouce 
add_subdirectory(src)

# third-party
add_subdirectory(third-party)

# aps-demo
if (BUILD_APS_DEMO)
    add_subdirectory(demo)
endif()