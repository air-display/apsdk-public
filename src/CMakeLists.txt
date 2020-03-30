# public headers
file(GLOB PUB_HEADERS
    aps.h
    ap_export.h
    ap_config.h
    ap_handler.h
    ap_server.h
    ap_session.h
    ap_types.h
)

# Source files
file(GLOB SRC_AP
    aps.h
    aps.cpp
    ap_export.h
    ap_server.h
    ap_server.cpp
    ap_config.h
    ap_config.cpp
    ap_handler.h
    ap_session.h
    ap_types.h
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
if (WINDOWS)
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
elseif (LINUX)
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
if (WIN32)
    list(APPEND SRC_UTILS
        utils/Windows/utils_impl.cpp
    )
elseif (APPLE)
    list(APPEND SRC_UTILS
        utils/macOS/utils_impl.cpp
    )
elseif (ANDROID)
    list(APPEND SRC_UTILS
        utils/Android/utils_impl.cpp
    )
endif()
source_group("utils" FILES ${SRC_UTILS})

add_definitions(-Daps_EXPORTS)

# Output
add_library(${PROJECT_NAME} SHARED
    ${SRC_AP}
    ${SRC_CRYPTO}
    ${SRC_NETWORK}
    ${SRC_MDNS}
    ${SRC_SERVICE}
    ${SRC_UTILS}
)

if (WINDOWS)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(APS_SDK_OUT ${CMAKE_SOURCE_DIR}/.out.win)
elseif (LINUX)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(APS_SDK_OUT ${CMAKE_SOURCE_DIR}/.out.linux)
elseif (ANDROID)

elseif (MACOSX)
    set(APS_SDK_OUT ${CMAKE_SOURCE_DIR}/.out.mac)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${APS_SDK_OUT}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${APS_SDK_OUT}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${APS_SDK_OUT}/bin"
)

if (APPLE)
    # Disalbe the ARC
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "NO")    
    
    # Build framework
    set_target_properties(${PROJECT_NAME} PROPERTIES
            FRAMEWORK TRUE
            PUBLIC_HEADER "${PUB_HEADERS}"
        )
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC
    curve25519
    ed25519
    hlsparser
    opensslaes
    playfair
)

# Dependencies
if (APPLE)
    find_library(FOUNDATION_FX Foundation)
    if (NOT FOUNDATION_FX)
        message(FATAL_ERROR "Foundation.Framework not found")
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FOUNDATION_FX})
elseif (ANDROID)
    target_link_libraries(${PROJECT_NAME} PUBLIC mdnssd log)
elseif (LINUX)
    # pthread is required by asio standalone
    target_link_libraries(${PROJECT_NAME} PUBLIC 
        pthread
        dns_sd
    )
endif()