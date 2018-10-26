project(aps)

file(GLOB_RECURSE SRC_FILES
    "./*.c"
    "./*.h"
)

add_library(ed25519 STATIC
    ${SRC_FILES}
)