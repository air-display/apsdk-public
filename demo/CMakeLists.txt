#################################################
# fdk-aac
add_subdirectory(fdk-aac)
if(WIN32)
    set_target_properties(fdk-aac PROPERTIES FOLDER demo)
endif(WIN32)

#################################################
# audiodec
file(GLOB_RECURSE audiodec_SRC_FILES
    "./audiodec/*.cpp"
    "./audiodec/*.h"
)
add_library(audiodec STATIC ${audiodec_SRC_FILES})
target_link_libraries(audiodec PRIVATE fdk-aac)
if(WIN32)
    set_target_properties(audiodec PROPERTIES FOLDER demo)
endif(WIN32)

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
if(WIN32)
    set_target_properties(aps-demo PROPERTIES FOLDER demo)
endif(WIN32)

