project(aps)

file(GLOB_RECURSE SRC_FILES
    "./*.cpp"
    "./*.h"
)

add_executable(aps-demo
    ${SRC_FILES}
)

set(depend_libs
    aps
    ed25519
    curve25519
    playfair
)

target_link_libraries(aps-demo PRIVATE
    ${depend_libs}
)
