cmake_minimum_required(VERSION 3.18)
project(whisper_jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to whisper.cpp source (cloned by CI)
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/../whisper.cpp)
set(WHISPER_LIB_PATH ${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so)

message(STATUS "Building for ABI: ${ANDROID_ABI}")
message(STATUS "Whisper library path: ${WHISPER_LIB_PATH}")

# Add whisper library as imported
add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES
    IMPORTED_LOCATION ${WHISPER_LIB_PATH}
)

# JNI bridge library
add_library(whisper_jni SHARED
    whisper_jni.cpp
)

# Include directories - whisper.h is in include/
target_include_directories(whisper_jni PRIVATE
    ${WHISPER_DIR}/include
)

target_link_libraries(whisper_jni
    whisper
    log
)
