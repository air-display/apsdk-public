project(aps)

add_library(mdnssd STATIC
    ./mDNSShared/dnssd_clientlib.c
    ./mDNSShared/dnssd_clientstub.c
    ./mDNSShared/dnssd_ipc.c
)
target_compile_definitions(mdnssd PRIVATE
    HAVE_IPV6
    NOT_HAVE_SA_LEN
    PLATFORM_NO_RLIMIT
    MDNS_DEBUGMSGS=0
    MDNS_UDS_SERVERPATH=\"/dev/socket/mdnsd\"
    MDNS_USERNAME=\"mdnsr\"
)