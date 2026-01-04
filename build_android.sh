#!/bin/bash
# Build whisper.cpp native libraries for Android
# Requires: Android NDK, CMake 3.18+

set -e

# Configuration
NDK_PATH="${ANDROID_NDK_HOME:-$HOME/Android/Sdk/ndk/25.2.9519653}"
BUILD_DIR="build_android"
OUTPUT_DIR="libs"
ABIS="arm64-v8a armeabi-v7a x86_64 x86"

echo "========================================"
echo "Whisper.cpp Android Build Script"
echo "========================================"
echo "NDK: $NDK_PATH"
echo "ABIs: $ABIS"
echo ""

# Check NDK
if [ ! -d "$NDK_PATH" ]; then
    echo "ERROR: Android NDK not found at $NDK_PATH"
    echo "Set ANDROID_NDK_HOME environment variable"
    exit 1
fi

# Clone whisper.cpp if not present
if [ ! -d "whisper.cpp" ]; then
    echo "Cloning whisper.cpp..."
    git clone https://github.com/ggerganov/whisper.cpp.git
fi

# Create output directory
mkdir -p $OUTPUT_DIR

cd whisper.cpp

for ABI in $ABIS; do
    echo ""
    echo "========================================"
    echo "Building for $ABI..."
    echo "========================================"
    
    mkdir -p $BUILD_DIR/$ABI
    cd $BUILD_DIR/$ABI
    
    cmake ../.. \
        -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=$ABI \
        -DANDROID_PLATFORM=android-24 \
        -DCMAKE_BUILD_TYPE=Release \
        -DWHISPER_BUILD_EXAMPLES=OFF \
        -DWHISPER_BUILD_TESTS=OFF \
        -DBUILD_SHARED_LIBS=ON
    
    cmake --build . --config Release -j$(nproc)
    
    cd ../..
    
    # Copy output
    mkdir -p ../$OUTPUT_DIR/$ABI
    cp $BUILD_DIR/$ABI/libwhisper.so ../$OUTPUT_DIR/$ABI/
    
    echo "Built: $OUTPUT_DIR/$ABI/libwhisper.so"
done

cd ..

echo ""
echo "========================================"
echo "Build Complete!"
echo "========================================"
echo ""
echo "Libraries are in: $OUTPUT_DIR/"
ls -la $OUTPUT_DIR/*/

echo ""
echo "Next steps:"
echo "1. Build the JNI bridge (see jni/)"
echo "2. Create a GitHub release with the .so files"
