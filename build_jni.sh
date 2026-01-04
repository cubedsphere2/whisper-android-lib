#!/bin/bash
# Build the JNI bridge library for Android
# Run this AFTER build_android.sh has built libwhisper.so

set -e

NDK_PATH="${ANDROID_NDK_HOME:-$HOME/Android/Sdk/ndk/25.2.9519653}"
OUTPUT_DIR="libs"
ABIS="arm64-v8a armeabi-v7a x86_64 x86"

echo "========================================"
echo "Building JNI Bridge"
echo "========================================"

# Check that whisper was built
if [ ! -f "whisper.cpp/build_android/arm64-v8a/libwhisper.so" ]; then
    echo "ERROR: libwhisper.so not found. Run build_android.sh first!"
    exit 1
fi

cd jni

for ABI in $ABIS; do
    echo ""
    echo "Building JNI for $ABI..."
    
    BUILD_DIR="build_$ABI"
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=$NDK_PATH/build/cmake/android.toolchain.cmake \
        -DANDROID_ABI=$ABI \
        -DANDROID_PLATFORM=android-24 \
        -DCMAKE_BUILD_TYPE=Release
    
    cmake --build . --config Release
    
    cd ..
    
    # Copy to output
    cp $BUILD_DIR/libwhisper_jni.so ../$OUTPUT_DIR/$ABI/
    
    echo "Built: $OUTPUT_DIR/$ABI/libwhisper_jni.so"
done

cd ..

echo ""
echo "========================================"
echo "JNI Build Complete!"
echo "========================================"
echo ""
ls -la $OUTPUT_DIR/*/
