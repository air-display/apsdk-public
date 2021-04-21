#################################################
# fdk-aac
add_subdirectory(fdk-aac)
if(WINDOWS)
    set_target_properties(fdk-aac PROPERTIES FOLDER demo)
endif(WINDOWS)

#################################################
# audiodec
file(GLOB_RECURSE audiodec_SRC_FILES
    "./audiodec/*.cpp"
    "./audiodec/*.h"
)
add_library(audiodec STATIC ${audiodec_SRC_FILES})
target_link_libraries(audiodec PRIVATE fdk-aac)
if(WINDOWS)
    set_target_properties(audiodec PROPERTIES FOLDER demo)
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
  audiodec
)
if(WINDOWS)
    set_target_properties(aps-demo PROPERTIES FOLDER demo)
endif(WINDOWS)

add_custom_command(TARGET aps-demo POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:aps> $<TARGET_FILE_DIR:aps-demo>
)