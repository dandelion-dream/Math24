#include <jni.h>
#include <string>
#include <vector>
#include <__format/format_functions.h>
#include "globals.hpp"
#include "JNI-interface.h"
#include "aec/AEC.hpp"

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
            jstring js = static_cast<jstring>(jo);
            input_numbers[i] = jstringToString(env, js);
        } else {

        }
    }
    std::string str_target = jstringToString(env, target);

    std::string result = AEC_solve(&input_numbers, str_target);

    jstring js_result = env->NewStringUTF(result.c_str());

    return js_result;
}

JNIEXPORT jobject JNICALL
Java_dandeliondream_math24_threads_AECProgressThread_getAECProgress(JNIEnv *env, jobject thiz) {
    double percentage = (static_cast<double>(aec_iterate_progress)
                        / static_cast<double>(aec_iterate_total))*100;
    std::string progress = std::format("{}/{}\n{}/{}\n{:.4f}%",
                                       aec_solve_progress.load(), aec_solve_total.load(),
                                       aec_iterate_progress.load(), aec_iterate_total.load(),
                                       percentage);
    jstring jprogress = env->NewStringUTF(progress.c_str());
    jclass clazzAECProgress = env->FindClass("dandeliondream/math24/services/AECProgress");
    jmethodID constructorAECProgress = env->GetMethodID(clazzAECProgress, "<init>",
                                                        "(Ljava/lang/String;D)V");
    jobject result = env->NewObject(clazzAECProgress, constructorAECProgress, jprogress,
                                    percentage);
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
    jclass clazzAECAnswer = env->FindClass("dandeliondream/math24/services/AECQuetion");
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