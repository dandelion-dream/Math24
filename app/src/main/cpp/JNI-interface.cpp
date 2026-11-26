#include <jni.h>
#include <string>
#include <vector>
#include <__format/format_functions.h>
#include "globals.hpp"
#include "JNI-interface.hpp"
#include "aec/AEC.hpp"

using std::vector;
using std::string;

extern "C" {
JNIEXPORT jstring JNICALL
Java_dandeliondream_math24_threads_AECMainThread_solveAEC
        (JNIEnv *env,
         jobject thiz,
         jobjectArray numbers,
         jstring target) {

    jsize jsize_input = env->GetArrayLength(numbers);
    size_t size_input = 0;
    if (jsize_input >= 0) {
        size_input = static_cast<size_t>(jsize_input);
    } else {

    }

    jclass stringClass = env->FindClass("java/lang/String");
    vector<std::string> input_numbers(size_input);
    for (size_t i = 0; i < size_input; i++) {
        jobject jo = env->GetObjectArrayElement(numbers, static_cast<jsize>(i));
        jboolean isString = env->IsInstanceOf(jo, stringClass);
        if (isString == JNI_TRUE) {
            auto js = static_cast<jstring>(jo);
            input_numbers[i] = jstringToString(env, js);
        } else {

        }
    }
    std::string str_target = jstringToString(env, target);

    std::string result = AEC_solve(input_numbers, str_target);

    jstring js_result = env->NewStringUTF(result.c_str());

    return js_result;
}

JNIEXPORT jobject JNICALL
Java_dandeliondream_math24_threads_AECProgressThread_getAECProgress(JNIEnv *env, jobject thiz) {
    jclass clazzAECProgress = env->FindClass("dandeliondream/math24/services/AECProgress");
    jmethodID constructorAECProgress = env->GetMethodID(clazzAECProgress, "<init>",
                                                        "(JJJJJJ)V");
    auto jroundTotal = static_cast<jlong>(aec_round_total);
    auto jroundFinished = static_cast<jlong>(aec_round_finished);
    auto jbucketTotal = static_cast<jlong>(aec_buckets_total);
    auto jbucketFilled = static_cast<jlong>(aec_buckets_filled);
    auto jcombTotal = static_cast<jlong>(aec_comb_total);
    auto jcombFinished = static_cast<jlong>(aec_comb_finished);
    jobject result = env->NewObject(clazzAECProgress, constructorAECProgress,
                                    jroundTotal,jroundFinished,jbucketTotal,jbucketFilled,jcombTotal,jcombFinished);
    return result;
}

JNIEXPORT jobject JNICALL
Java_dandeliondream_math24_threads_AECMainThread_generateAEC(JNIEnv *env, jobject thiz,
                                                             jstring boundary, jstring sz,
                                                             jstring target) {
    string s1 = jstringToString(env, boundary);
    string s2 = jstringToString(env, sz);
    string s3 = jstringToString(env, target);
    aecAnswer answer(AEC_generate(s1, s2, s3));
    jclass clazzAECAnswer = env->FindClass("dandeliondream/math24/services/AECQuestion");
    jmethodID constructorAECAnswer = env->GetMethodID(clazzAECAnswer, "<init>",
                                                      "(Ljava/lang/String;Ljava/lang/String;)V");
    jstring jquestion = env->NewStringUTF(answer.question.c_str());
    jstring janswer = env->NewStringUTF(answer.answer.c_str());
    jobject result = env->NewObject(clazzAECAnswer, constructorAECAnswer, jquestion, janswer);
    return result;
}
JNIEXPORT void JNICALL
Java_dandeliondream_math24_services_BackgroundService_setRunningState(JNIEnv *env, jobject thiz,
                                                                      jboolean state) {
    keep_running = static_cast<bool>(state);
}
}

std::string jstringToString(JNIEnv *env, jstring jstr) {
    if (env == nullptr || jstr == nullptr) {
        return "";
    }
    const char *chars = env->GetStringUTFChars(jstr, nullptr);
    if (chars == nullptr) {
        return "";
    }
    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return result;
}