project(aps)

file(GLOB_RECURSE SRC_FILES
    "./*.c"
    "./*.h"
)

add_library(libed25519 STATIC
    ${SRC_FILES}
)