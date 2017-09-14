//
// Created by Administrator on 9/11/2017.
//
#include <string>
#include <cstdint>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>

#include "BitMailWrapper.h"

#include "x509cert.h"

JNIEXPORT jstring JNICALL
Java_com_hyphenate_easeui_EaseUI_NativeJsonRPC(JNIEnv *env, jclass type, jstring payload_) {
    const char *payload = env->GetStringUTFChars(payload_, 0);

    // TODO

    env->ReleaseStringUTFChars(payload_, payload);

    return env->NewStringUTF("{}");
}