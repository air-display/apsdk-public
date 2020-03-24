project(aps)

# Source files
file(GLOB SRC_AP
    aps.h
    aps.cpp
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
    target_link_libraries(aps PUBLIC ${FOUNDATION_FX})
elseif (ANDROID)
    target_link_libraries(aps PUBLIC mdnssd)
elseif (LINUX)
    # pthread is required by asio standalone
    target_link_libraries(aps PUBLIC 
        pthread
        dns_sd
    )
endif()