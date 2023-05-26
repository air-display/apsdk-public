project(aps)

set(SRC_AP_PLATFORM)

if(APPLE)
    # public headers
    file(GLOB PUB_HEADERS
        ${AP_PLATFORM_SOURCE}/apple/APSDK.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlayServer.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlayConfig.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlaySession.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlaySessionDelegate.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlayMirroringDelegate.h
        ${AP_PLATFORM_SOURCE}/apple/APAirPlayCastingDelegate.h
    )
    source_group("include" FILES ${PUB_HEADERS})

    file(GLOB SRC_AP_PLATFORM
        ${AP_PLATFORM_SOURCE}/apple/*.m
        ${AP_PLATFORM_SOURCE}/apple/*.mm
    )
    list(APPEND SRC_AP_PLATFORM)
    source_group("apoc" FILES ${SRC_AP_PLATFORM})
else()
    # public headers
    file(GLOB PUB_HEADERS
        ../include/aps.h
        ../include/ap_export.h
        ../include/ap_config.h
        ../include/ap_handler.h
        ../include/ap_server.h
        ../include/ap_session.h
        ../include/ap_types.h
    )
    source_group("include" FILES ${PUB_HEADERS})
endif()

# Source files
file(GLOB SRC_AP
    aps.cpp
    ap_config.cpp
    ap_server.cpp
)
source_group("ap" FILES ${SRC_AP})

file(GLOB SRC_CRYPTO
    crypto/*.h
    crypto/*.cpp
)
source_group("crypto" FILES ${SRC_CRYPTO})

file(GLOB SRC_MDNS
    mdns/*.h
    mdns/*.cpp
)

if(WINDOWS)
    list(APPEND SRC_MDNS
        mdns/dns_sd.h
        mdns/Windows/net_service_impl.cpp
    )
elseif(APPLE)
    list(APPEND SRC_MDNS
        mdns/macOS/net_service_impl.mm
    )
elseif(ANDROID)
    list(APPEND SRC_MDNS
        mdns/dns_sd.h
        mdns/Android/net_service_impl.cpp
    )
elseif(LINUX)
    list(APPEND SRC_MDNS
        mdns/Linux/net_service_impl.cpp
    )
endif()

source_group("mdns" FILES ${SRC_MDNS})

file(GLOB SRC_NETWORK
    network/*.h
    network/*.cpp
)
source_group("network" FILES ${SRC_NETWORK})

file(GLOB SRC_SERVICE
    service/*.cpp
    service/*.h
)
source_group("service" FILES ${SRC_SERVICE})

file(GLOB SRC_UTILS
    utils/*.h
    utils/*.cpp
)

if(WIN32)
    list(APPEND SRC_UTILS
        utils/Windows/utils_impl.cpp
    )
elseif(APPLE)
    list(APPEND SRC_UTILS
        utils/macOS/utils_impl.cpp
    )
elseif(ANDROID)
    list(APPEND SRC_UTILS
        utils/Android/utils_impl.cpp
    )
endif()

source_group("utils" FILES ${SRC_UTILS})

if(BUILD_APS_STATIC)
    set(APS_LIBRARY_TYPE STATIC)
else()
    set(APS_LIBRARY_TYPE SHARED)
endif()

# Output
add_library(${PROJECT_NAME} ${APS_LIBRARY_TYPE}
    ${PUB_HEADERS}
    ${SRC_AP_PLATFORM}
    ${SRC_AP}
    ${SRC_CRYPTO}
    ${SRC_NETWORK}
    ${SRC_MDNS}
    ${SRC_SERVICE}
    ${SRC_UTILS}
)

if(BUILD_APS_STATIC)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        -DAPS_STATIC_LIB
    )
else()
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        -Daps_EXPORTS
    )
endif()

if(WINDOWS)
    set(CMAKE_CXX_STANDARD 17)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        -D_CRT_SECURE_NO_WARNINGS
    )
endif()

if(LINUX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(APPLE)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
        -DAPS_STATIC_LIB
    )
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
    .
    ../include
)

set(APS_SDK_OUT ${CMAKE_BINARY_DIR}/output)
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${APS_SDK_OUT}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${APS_SDK_OUT}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${APS_SDK_OUT}/bin"
)

if(APPLE)
    message(STATUS "++++++++++ adding configs for APPLE target systems")

    # Build framework
    set_target_properties(${PROJECT_NAME} PROPERTIES
        FRAMEWORK TRUE
        PUBLIC_HEADER "${PUB_HEADERS}"
    )
endif()

add_dependencies(${PROJECT_NAME}
    curve25519
    ed25519
    hlsparser
    opensslaes
    fairplay
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    curve25519
    ed25519
    hlsparser
    opensslaes
    fairplay
)

# Dependencies
if(APPLE)
    message(STATUS "++++++++++ adding dependencies for APPLE target systems")
    find_library(FOUNDATION_FX Foundation)

    if(NOT FOUNDATION_FX)
        message(FATAL_ERROR "Foundation.Framework not found")
    endif()

    target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION_FX})
elseif(ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC mdnssd log)
elseif(LINUX)
    # pthread is required by asio standalone
    target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
        dns_sd
    )
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER apsdk
)