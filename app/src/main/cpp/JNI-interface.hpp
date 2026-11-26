//
// Created by alfred on 2026/11/22.
//

#ifndef MATH24_JNI_INTERFACE_HPP
#define MATH24_JNI_INTERFACE_HPP

#include <jni.h>
#include <string>

std::string jstringToString(JNIEnv *env, jstring jstr);

#endif //MATH24_JNI_INTERFACE_HPP
