#include <jni.h>
#include <string>
#include "whisper.h"

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_bberumen_securenotes_speech_WhisperJni_nativeCreateContext(
    JNIEnv *env, jobject thiz, jstring model_path) {
    const char *path = env->GetStringUTFChars(model_path, nullptr);
    
    // Use the proper API with context params
    struct whisper_context_params cparams = whisper_context_default_params();
    struct whisper_context *ctx = whisper_init_from_file_with_params(path, cparams);
    
    env->ReleaseStringUTFChars(model_path, path);
    return reinterpret_cast<jlong>(ctx);
}

JNIEXPORT jstring JNICALL
Java_com_bberumen_securenotes_speech_WhisperJni_nativeTranscribe(
    JNIEnv *env, jobject thiz, jlong context_ptr, jfloatArray samples) {
    struct whisper_context *ctx = reinterpret_cast<struct whisper_context *>(context_ptr);
    if (!ctx) {
        return env->NewStringUTF("");
    }

    jfloat *data = env->GetFloatArrayElements(samples, nullptr);
    jsize len = env->GetArrayLength(samples);
    
    // Set up transcription params with greedy sampling
    struct whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.print_progress = false;
    wparams.print_special = false;
    wparams.print_realtime = false;
    wparams.print_timestamps = false;
    wparams.single_segment = true;
    wparams.language = "en";
    
    // Run transcription
    int result = whisper_full(ctx, wparams, data, len);
    env->ReleaseFloatArrayElements(samples, data, 0);
    
    if (result != 0) {
        return env->NewStringUTF("");
    }
    
    // Get result text
    std::string text;
    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char *segment = whisper_full_get_segment_text(ctx, i);
        if (segment) {
            text += segment;
        }
    }
    
    return env->NewStringUTF(text.c_str());
}

JNIEXPORT void JNICALL
Java_com_bberumen_securenotes_speech_WhisperJni_nativeFreeContext(
    JNIEnv *env, jobject thiz, jlong context_ptr) {
    struct whisper_context *ctx = reinterpret_cast<struct whisper_context *>(context_ptr);
    if (ctx) {
        whisper_free(ctx);
    }
}

} // extern "C"
