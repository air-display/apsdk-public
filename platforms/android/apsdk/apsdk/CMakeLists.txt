# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.18)
project(aps-jni)

# Include the aps directory to current source directory as a virtual directory
set(BUILD_APS_STATIC ON)
add_subdirectory(../../../.. apsdk)

include_directories(
	src/main/cpp
	../../../../include
    ../../../../src
)

file(GLOB_RECURSE SRC_FILES
    "src/main/cpp/*.cpp"
    "src/main/cpp/*.h"
)

add_library(${PROJECT_NAME} SHARED
	${SRC_FILES}
)

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		-DAPS_STATIC_LIB
)

find_library(log-lib log)

target_link_libraries(${PROJECT_NAME}
	PRIVATE
		aps
		${log-lib}
)

