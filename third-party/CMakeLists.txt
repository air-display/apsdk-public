project(aps-third-party)

#################################################
add_subdirectory(fdk-aac)

#################################################
file(GLOB_RECURSE curve25519_SRC_FILES
    "./curve25519/*.c"
    "./curve25519/*.h"
)
add_library(curve25519 STATIC ${curve25519_SRC_FILES})
if(WIN32)
    set_target_properties(curve25519 PROPERTIES FOLDER thrid-party)
endif(WIN32)

#################################################
file(GLOB_RECURSE ed25519_SRC_FILES
    "./ed25519/*.c"
    "./ed25519/*.h"
)
add_library(ed25519 STATIC ${ed25519_SRC_FILES})
if(WIN32)
    set_target_properties(ed25519 PROPERTIES FOLDER thrid-party)
endif(WIN32)

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
add_library(playfair STATIC ${playfair_SRC_FILES})
if(WIN32)
    set_target_properties(playfair PROPERTIES FOLDER thrid-party)
endif(WIN32)

#################################################
file(GLOB_RECURSE hlsparser_SRC_FILES
    "./hlsparser/*.c"
    "./hlsparser/*.h"
)
add_library(hlsparser STATIC ${hlsparser_SRC_FILES})
if(WIN32)
    set_target_properties(hlsparser PROPERTIES FOLDER thrid-party)
endif(WIN32)

#################################################
file(GLOB_RECURSE opensslaes_SRC_FILES
    "./opensslaes/*.c"
    "./opensslaes/*.h"
)
add_library(opensslaes STATIC ${opensslaes_SRC_FILES})
if(WIN32)
    set_target_properties(opensslaes PROPERTIES FOLDER thrid-party)
endif(WIN32)

#################################################
file(GLOB_RECURSE audiodec_SRC_FILES
    "./audiodec/*.cpp"
    "./audiodec/*.h"
)
add_library(audiodec STATIC ${audiodec_SRC_FILES})
target_link_libraries(audiodec PRIVATE fdk-aac)
if(WIN32)
    set_target_properties(audiodec PROPERTIES FOLDER thrid-party)
endif(WIN32)