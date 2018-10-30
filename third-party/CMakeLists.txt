project(aps)

#################################################
file(GLOB_RECURSE curve25519_SRC_FILES
    "./curve25519/*.c"
    "./curve25519/*.h"
)
add_library(curve25519 STATIC
    ${curve25519_SRC_FILES}
)

#################################################
file(GLOB_RECURSE ed25519_SRC_FILES
    "./ed25519/*.c"
    "./ed25519/*.h"
)
add_library(ed25519 STATIC
    ${ed25519_SRC_FILES}
)

#################################################
if (ANDROID)
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

#################################################
file(GLOB_RECURSE playfair_SRC_FILES
    "./playfair/*.c"
    "./playfair/*.h"
)
add_library(playfair STATIC
    ${playfair_SRC_FILES}
)