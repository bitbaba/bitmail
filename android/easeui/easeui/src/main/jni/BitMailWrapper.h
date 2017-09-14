//
// Created by Administrator on 9/11/2017.
//

#ifndef CHATDEMOUI3_0_BITMAILWRAPPER_H
#define CHATDEMOUI3_0_BITMAILWRAPPER_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring JNICALL
Java_com_hyphenate_easeui_EaseUI_NativeJsonRPC(JNIEnv *env, jclass type, jstring payload_);

#ifdef __cplusplus
};
#endif

#endif //CHATDEMOUI3_0_BITMAILWRAPPER_H
