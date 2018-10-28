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

if (APPLE)
    find_library(CORE_FOUNDATION Foundation)

    if (NOT CORE_FOUNDATION)
        message(FATAL_ERROR "CoreFoundation not found")
    endif()

    list(APPEND depend_libs
        ${depens}
        ${CORE_FOUNDATION}
    )
endif()

target_link_libraries(aps-demo PRIVATE
    ${depend_libs}
)
