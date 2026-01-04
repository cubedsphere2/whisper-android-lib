cmake_minimum_required(VERSION 3.18)
project(whisper_jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to whisper.cpp source (cloned in parent directory by CI)
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/../whisper.cpp)
set(WHISPER_BUILD_DIR ${WHISPER_DIR}/build/${ANDROID_ABI})

# Debug: print paths and list directory
message(STATUS "WHISPER_DIR: ${WHISPER_DIR}")
message(STATUS "WHISPER_BUILD_DIR: ${WHISPER_BUILD_DIR}")
message(STATUS "ANDROID_ABI: ${ANDROID_ABI}")

# List files in build directory
file(GLOB BUILD_FILES "${WHISPER_BUILD_DIR}/*")
message(STATUS "Files in build dir: ${BUILD_FILES}")

# Find the whisper library (might be named differently)
file(GLOB WHISPER_LIB_FILES "${WHISPER_BUILD_DIR}/libwhisper*.so")
message(STATUS "Found whisper libraries: ${WHISPER_LIB_FILES}")

# Use the first found library
list(GET WHISPER_LIB_FILES 0 WHISPER_LIB_PATH)
message(STATUS "Using whisper library: ${WHISPER_LIB_PATH}")

# Add whisper library as imported
add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES
    IMPORTED_LOCATION ${WHISPER_LIB_PATH}
)

# JNI bridge library
add_library(whisper_jni SHARED
    whisper_jni.cpp
)

# Include directories
target_include_directories(whisper_jni PRIVATE
    ${WHISPER_DIR}/include
    ${WHISPER_DIR}/ggml/include
    ${WHISPER_DIR}
)

target_link_libraries(whisper_jni
    whisper
    log
    android
)
