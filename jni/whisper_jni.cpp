#include <jni.h>
#include <string>
#include <android/log.h>
#include "whisper.h"

#define TAG "WhisperJNI"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeCreateContext(
        JNIEnv *env,
        jobject thiz,
        jstring modelPath) {
    (void)thiz;
    
    const char *path = env->GetStringUTFChars(modelPath, nullptr);
    LOGI("Loading Whisper model from: %s", path);
    
    struct whisper_context *ctx = whisper_init_from_file(path);
    
    env->ReleaseStringUTFChars(modelPath, path);
    
    if (ctx == nullptr) {
        LOGE("Failed to initialize whisper context");
        return 0;
    }
    
    LOGI("Whisper context created successfully");
    return (jlong) ctx;
}

JNIEXPORT jstring JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeTranscribe(
        JNIEnv *env,
        jobject thiz,
        jlong contextPtr,
        jfloatArray samples,
        jstring language,
        jboolean translate) {
    (void)thiz;
    
    if (contextPtr == 0) {
        LOGE("Invalid context pointer");
        return env->NewStringUTF("");
    }
    
    struct whisper_context *ctx = (struct whisper_context *) contextPtr;
    
    // Get audio samples
    jsize numSamples = env->GetArrayLength(samples);
    jfloat *audioData = env->GetFloatArrayElements(samples, nullptr);
    
    LOGI("Transcribing %d samples", numSamples);
    
    // Get language
    const char *lang = env->GetStringUTFChars(language, nullptr);
    
    // Set up whisper parameters
    struct whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_realtime = false;
    params.print_progress = false;
    params.print_timestamps = false;
    params.print_special = false;
    params.translate = translate;
    params.language = (strcmp(lang, "auto") == 0) ? "en" : lang;
    params.n_threads = 4;
    
    // Run transcription
    int result = whisper_full(ctx, params, audioData, numSamples);
    
    env->ReleaseFloatArrayElements(samples, audioData, 0);
    env->ReleaseStringUTFChars(language, lang);
    
    if (result != 0) {
        LOGE("Whisper transcription failed with code: %d", result);
        return env->NewStringUTF("");
    }
    
    // Collect results
    std::string fullText;
    int numSegments = whisper_full_n_segments(ctx);
    
    for (int i = 0; i < numSegments; i++) {
        const char *segmentText = whisper_full_get_segment_text(ctx, i);
        if (segmentText) {
            fullText += segmentText;
        }
    }
    
    LOGI("Transcription complete: %d segments", numSegments);
    
    return env->NewStringUTF(fullText.c_str());
}

JNIEXPORT jstring JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeTranscribeFile(
        JNIEnv *env,
        jobject thiz,
        jlong contextPtr,
        jstring wavPath,
        jstring language,
        jboolean translate) {
    (void)thiz;
    (void)contextPtr;
    (void)wavPath;
    (void)language;
    (void)translate;
    
    return env->NewStringUTF("");
}

JNIEXPORT void JNICALL
Java_com_securenotes_app_util_WhisperJni_nativeFreeContext(
        JNIEnv *env,
        jobject thiz,
        jlong contextPtr) {
    (void)env;
    (void)thiz;
    
    if (contextPtr != 0) {
        struct whisper_context *ctx = (struct whisper_context *) contextPtr;
        whisper_free(ctx);
        LOGI("Whisper context freed");
    }
}

} // extern "C"
