#################################################
project(apsdk-demo)
file(GLOB SRC_FILES
    "./*.cpp"
    "./*.h"
    "./*.hpp"
)
add_executable(${PROJECT_NAME}
  ${SRC_FILES}
)

target_link_libraries(${PROJECT_NAME}  
  PRIVATE
    aps
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    FOLDER demo
)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:aps> $<TARGET_FILE_DIR:${PROJECT_NAME}>
)