# Whisper Android Native Libraries

Pre-built whisper.cpp native libraries for Android. Enables offline speech-to-text transcription using OpenAI's Whisper model.

## 📦 Download

Download the latest release from the [Releases](https://github.com/cubedsphere2/whisper-android-lib/releases) page.

### Available Architectures

| Architecture | Description | File |
|-------------|-------------|------|
| `arm64-v8a` | 64-bit ARM (most modern Android phones) | `libwhisper_arm64-v8a.so` |
| `armeabi-v7a` | 32-bit ARM (older devices) | `libwhisper_armeabi-v7a.so` |
| `x86_64` | 64-bit x86 (emulators) | `libwhisper_x86_64.so` |
| `x86` | 32-bit x86 (old emulators) | `libwhisper_x86.so` |

For each architecture, you need both:
- `libwhisper_{ABI}.so` - The whisper.cpp core library
- `libwhisper_jni_{ABI}.so` - JNI bridge for Java/Kotlin

## 🚀 Usage

These libraries are designed for use with [SecureNotes](https://github.com/cubedsphere2/SecureNotes) but can be integrated into any Android app.

### Integration

1. Download the appropriate `.so` files for your target architectures
2. Place them in your app's `jniLibs` folder or download at runtime
3. Load with `System.load(path)` - load `libwhisper.so` first, then `libwhisper_jni.so`

### Runtime Download (Recommended)

SecureNotes downloads these libraries on-demand to keep the APK size small:

```kotlin
// Download to app's private storage
val libDir = File(context.filesDir, "whisper_native/arm64-v8a")
System.load("${libDir}/libwhisper.so")
System.load("${libDir}/libwhisper_jni.so")
```

## 📊 Library Sizes

| Architecture | libwhisper.so | libwhisper_jni.so | Total |
|-------------|---------------|-------------------|-------|
| arm64-v8a | ~3.5 MB | ~50 KB | ~3.5 MB |
| armeabi-v7a | ~2.5 MB | ~40 KB | ~2.5 MB |
| x86_64 | ~4.0 MB | ~60 KB | ~4.0 MB |
| x86 | ~3.0 MB | ~50 KB | ~3.0 MB |

## 🔧 Building from Source

### Prerequisites

- Android NDK r25c or later
- CMake 3.18+
- Git

### Quick Build

```bash
./build_android.sh
```

### Manual Build

1. Clone whisper.cpp:
```bash
git clone https://github.com/ggerganov/whisper.cpp.git
cd whisper.cpp
```

2. Build for each architecture:
```bash
mkdir -p build/arm64-v8a && cd build/arm64-v8a
cmake ../.. \
    -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-24 \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_SHARED_LIBS=ON
cmake --build . --config Release
```

## 📋 JNI API

The JNI bridge exposes these native functions:

```kotlin
// Package: com.securenotes.app.util.WhisperJni

// Create context from GGML model file
external fun nativeCreateContext(modelPath: String): Long

// Transcribe audio samples (16kHz, mono, float32 normalized -1.0 to 1.0)
external fun nativeTranscribe(
    contextPtr: Long,
    samples: FloatArray,
    language: String,    // "en", "es", "auto", etc.
    translate: Boolean   // true to translate to English
): String?

// Free context when done
external fun nativeFreeContext(contextPtr: Long)
```

## 🔗 Related Projects

- [whisper.cpp](https://github.com/ggerganov/whisper.cpp) - The original C/C++ implementation
- [SecureNotes](https://github.com/cubedsphere2/SecureNotes) - Privacy-focused note-taking app

## 📄 License

Built from [whisper.cpp](https://github.com/ggerganov/whisper.cpp) (MIT License).
