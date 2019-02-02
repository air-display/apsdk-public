project(aps)

file(GLOB_RECURSE SRC_FILES
    "./*.cpp"
    "./*.h"
    "./*.hpp"
)

add_executable(aps-demo
    ${SRC_FILES}
)

target_link_libraries(aps-demo PRIVATE
  aps
  audiodec
)

