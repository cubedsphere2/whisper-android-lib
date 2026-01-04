cmake_minimum_required(VERSION 3.18)
project(whisper_jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to whisper.cpp source (cloned in parent directory by CI)
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/../whisper.cpp)

# Debug: print paths
message(STATUS "WHISPER_DIR: ${WHISPER_DIR}")
message(STATUS "ANDROID_ABI: ${ANDROID_ABI}")
message(STATUS "Looking for library at: ${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so")

# Check if library exists
if(NOT EXISTS "${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so")
    message(FATAL_ERROR "libwhisper.so not found at ${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so")
endif()

# Add whisper library as imported
add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES
    IMPORTED_LOCATION ${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so
)

# JNI bridge library
add_library(whisper_jni SHARED
    whisper_jni.cpp
)

# Include directories - whisper.h is in include/
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
