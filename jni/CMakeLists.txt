cmake_minimum_required(VERSION 3.18)
project(whisper_jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to whisper.cpp source
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/../whisper.cpp)

# Add whisper library
add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES
    IMPORTED_LOCATION ${WHISPER_DIR}/build_android/${ANDROID_ABI}/libwhisper.so
)

# JNI bridge library
add_library(whisper_jni SHARED
    whisper_jni.cpp
)

target_include_directories(whisper_jni PRIVATE
    ${WHISPER_DIR}
    ${WHISPER_DIR}/include
)

target_link_libraries(whisper_jni
    whisper
    log
    android
)
