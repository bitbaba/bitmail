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
Java_com_hyphenate_chatuidemo_DemoApplication_NativeJsonRPC(JNIEnv *env, jobject instance, jstring payload_) ;

#ifdef __cplusplus
};
#endif

#endif //CHATDEMOUI3_0_BITMAILWRAPPER_H

extern "C"
JNIEXPORT jstring JNICALL
Java_com_hyphenate_easeui_EaseUI_NativeJsonRPC(JNIEnv *env, jclass type, jstring payload_) {
    const char *payload = env->GetStringUTFChars(payload_, 0);

    // TODO

    env->ReleaseStringUTFChars(payload_, payload);

    return env->NewStringUTF(returnValue);
}