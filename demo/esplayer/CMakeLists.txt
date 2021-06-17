file(GLOB_RECURSE esplayer_SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(esplayer STATIC ${esplayer_SRC_FILES})

target_include_directories(esplayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/include
)

target_link_directories(esplayer PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../deps/$<CONFIG>/lib
)

set(esplayer_LINKED_LIBS
    SDL2
    avformat
    avcodec
    avutil
    swresample
    swscale
    fdk-aac
)

if(WINDOWS)
    list(APPEND esplayer_LINKED_LIBS
        Ws2_32
        Winmm
        Version
        Imm32
        Setupapi
        Secur32
        Bcrypt
        mfplat
        mfuuid
        strmiids
    )
endif(WINDOWS)

target_link_libraries(esplayer 
  PUBLIC 
    ${esplayer_LINKED_LIBS}
)