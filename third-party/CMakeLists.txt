project(aps-third-party)

# ################################################
file(GLOB_RECURSE curve25519_SRC_FILES
    "./curve25519/*.c"
    "./curve25519/*.h"
)
add_library(curve25519 STATIC ${curve25519_SRC_FILES})

# set_property(TARGET curve25519 PROPERTY INTERFACE_SOURCES ON)
set_target_properties(curve25519 PROPERTIES FOLDER apsdk/third-party)

# ################################################
file(GLOB_RECURSE ed25519_SRC_FILES
    "./ed25519/*.c"
    "./ed25519/*.h"
)
add_library(ed25519 STATIC ${ed25519_SRC_FILES})
set_target_properties(ed25519 PROPERTIES FOLDER apsdk/third-party)

# ################################################
if(ANDROID)
    add_library(mdnssd STATIC
        ./mdnsresponder/mDNSShared/dnssd_clientlib.c
        ./mdnsresponder/mDNSShared/dnssd_clientstub.c
        ./mdnsresponder/mDNSShared/dnssd_ipc.c
    )
    target_compile_definitions(mdnssd PRIVATE
        HAVE_IPV6
        NOT_HAVE_SA_LEN
        PLATFORM_NO_RLIMIT
        MDNS_DEBUGMSGS=0
        MDNS_UDS_SERVERPATH=\"/dev/socket/mdnsd\"
        MDNS_USERNAME=\"mdnsr\"
    )
endif()

# ################################################
file(GLOB_RECURSE hlsparser_SRC_FILES
    "./hlsparser/*.c"
    "./hlsparser/*.h"
)
add_library(hlsparser STATIC ${hlsparser_SRC_FILES})
set_target_properties(hlsparser PROPERTIES FOLDER apsdk/third-party)

# ################################################
file(GLOB_RECURSE opensslaes_SRC_FILES
    "./opensslaes/*.c"
    "./opensslaes/*.h"
)
add_library(opensslaes STATIC ${opensslaes_SRC_FILES})
set_target_properties(opensslaes PROPERTIES FOLDER apsdk/third-party)

# ################################################
add_subdirectory(fairplay)