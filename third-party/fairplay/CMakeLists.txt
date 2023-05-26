project(fairplay LANGUAGES C)

if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/details/fairplay.c)
    message(STATUS "Removing generated fairplay.c")
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/fairplay.c)
else()
    message(STATUS "Generating placeholder of fairplay.c")
    configure_file(
        fairplay.c.in
        ${CMAKE_CURRENT_BINARY_DIR}/fairplay.c
        COPYONLY)
endif()

file(GLOB_RECURSE fairplay_SRC_FILES
    "./*.c"
    "./*.h"
    "${CMAKE_CURRENT_BINARY_DIR}/*.c"
)
add_library(fairplay STATIC ${fairplay_SRC_FILES})
set_target_properties(fairplay PROPERTIES FOLDER apsdk/third-party)