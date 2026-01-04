#include <jni.h>
#include <string>
#include <android/log.h>
#include "whisper.h"

#define TAG "WhisperJNI"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern "C" {

/**
 * Create a whisper context from a GGML model file
 * 
 * @param modelPath Absolute path to the .bin model file
 * @return Context pointer (0 if failed)
 */
JNIEXPORT jlong JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeCreateContext(
        JNIEnv *env,
        jobject /* this */,
        jstring modelPath) {
    
    const char *path = env->GetStringUTFChars(modelPath, nullptr);
    LOGI("Loading Whisper model from: %s", path);
    
    struct whisper_context_params params = whisper_context_default_params();
    params.use_gpu = false;  // GPU not reliably supported on all Android devices
    
    struct whisper_context *ctx = whisper_init_from_file_with_params(path, params);
    
    env->ReleaseStringUTFChars(modelPath, path);
    
    if (ctx == nullptr) {
        LOGE("Failed to initialize whisper context");
        return 0;
    }
    
    LOGI("Whisper context created successfully");
    return reinterpret_cast<jlong>(ctx);
}

/**
 * Transcribe audio samples
 * 
 * @param contextPtr Whisper context pointer
 * @param samples Audio samples (16kHz, mono, float32, normalized -1.0 to 1.0)
 * @param language Language code ("en", "es", "auto" for auto-detect)
 * @param translate If true, translate to English
 * @return Transcribed text
 */
JNIEXPORT jstring JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeTranscribe(
        JNIEnv *env,
        jobject /* this */,
        jlong contextPtr,
        jfloatArray samples,
        jstring language,
        jboolean translate) {
    
    if (contextPtr == 0) {
        LOGE("Invalid context pointer");
        return env->NewStringUTF("");
    }
    
    struct whisper_context *ctx = reinterpret_cast<struct whisper_context *>(contextPtr);
    
    // Get audio samples
    jsize numSamples = env->GetArrayLength(samples);
    jfloat *audioData = env->GetFloatArrayElements(samples, nullptr);
    
    float durationSec = (float)numSamples / 16000.0f;
    LOGI("Transcribing %.1f seconds of audio (%d samples)", durationSec, numSamples);
    
    // Get language
    const char *lang = env->GetStringUTFChars(language, nullptr);
    
    // Set up whisper parameters for best mobile performance
    struct whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_realtime = false;
    params.print_progress = false;
    params.print_timestamps = false;
    params.print_special = false;
    params.translate = translate;
    params.language = (strcmp(lang, "auto") == 0) ? nullptr : lang;
    params.n_threads = 4;  // Good balance for mobile
    params.offset_ms = 0;
    params.no_context = true;
    params.single_segment = false;
    params.suppress_blank = true;
    params.suppress_non_speech_tokens = true;
    
    // Run transcription
    LOGD("Starting whisper_full...");
    int result = whisper_full(ctx, params, audioData, numSamples);
    
    env->ReleaseFloatArrayElements(samples, audioData, 0);
    env->ReleaseStringUTFChars(language, lang);
    
    if (result != 0) {
        LOGE("Whisper transcription failed with code: %d", result);
        return env->NewStringUTF("");
    }
    
    // Collect results from all segments
    std::string fullText;
    int numSegments = whisper_full_n_segments(ctx);
    
    for (int i = 0; i < numSegments; i++) {
        const char *segmentText = whisper_full_get_segment_text(ctx, i);
        if (segmentText) {
            fullText += segmentText;
        }
    }
    
    LOGI("Transcription complete: %d segments, %zu characters", numSegments, fullText.size());
    
    return env->NewStringUTF(fullText.c_str());
}

/**
 * Transcribe audio from a WAV file
 * (Not fully implemented - use sample array instead)
 */
JNIEXPORT jstring JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeTranscribeFile(
        JNIEnv *env,
        jobject /* this */,
        jlong contextPtr,
        jstring wavPath,
        jstring language,
        jboolean translate) {
    
    LOGD("nativeTranscribeFile called - use nativeTranscribe with sample array instead");
    return env->NewStringUTF("");
}

/**
 * Free a whisper context
 */
JNIEXPORT void JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeFreeContext(
        JNIEnv *env,
        jobject /* this */,
        jlong contextPtr) {
    
    if (contextPtr != 0) {
        struct whisper_context *ctx = reinterpret_cast<struct whisper_context *>(contextPtr);
        whisper_free(ctx);
        LOGI("Whisper context freed");
    }
}

} // extern "C"
