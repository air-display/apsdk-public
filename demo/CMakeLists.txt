#################################################
# es-player
add_subdirectory(esplayer)
if(WINDOWS)
    set_target_properties(esplayer PROPERTIES FOLDER demo)
endif(WINDOWS)

#################################################
# aps-demo
file(GLOB SRC_FILES
    "./*.cpp"
    "./*.h"
    "./*.hpp"
)
add_executable(aps-demo ${SRC_FILES})

target_link_libraries(aps-demo PRIVATE
  aps
  esplayer
)

if(WINDOWS)
    set_target_properties(aps-demo PROPERTIES FOLDER demo)
endif(WINDOWS)

add_custom_command(TARGET aps-demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:aps> $<TARGET_FILE_DIR:aps-demo>
)