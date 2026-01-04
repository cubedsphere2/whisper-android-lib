cmake_minimum_required(VERSION 3.18)
project(whisper_jni)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Path to whisper.cpp source
set(WHISPER_DIR ${CMAKE_SOURCE_DIR}/../whisper.cpp)

# The library is already built, we just need to link to it
set(WHISPER_LIB_PATH ${WHISPER_DIR}/build/${ANDROID_ABI}/libwhisper.so)

message(STATUS "Looking for whisper library at: ${WHISPER_LIB_PATH}")
message(STATUS "ANDROID_ABI: ${ANDROID_ABI}")

# Create imported library target
add_library(whisper SHARED IMPORTED)
set_target_properties(whisper PROPERTIES 
    IMPORTED_LOCATION ${WHISPER_LIB_PATH}
    IMPORTED_NO_SONAME TRUE
)

# Create the JNI library
add_library(whisper_jni SHARED whisper_jni.cpp)

# Add include directories - whisper.h is in include/
target_include_directories(whisper_jni PRIVATE 
    ${WHISPER_DIR}/include
    ${WHISPER_DIR}/ggml/include
)

# Link libraries
target_link_libraries(whisper_jni 
    whisper
    log
)
