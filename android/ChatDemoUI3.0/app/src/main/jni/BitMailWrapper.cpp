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
Java_com_hyphenate_chatuidemo_DemoApplication_NativeJsonRPC(JNIEnv *env
                                                            , jobject instance
                                                            , jstring payload_) {

    {
        OPENSSL_load_builtin_modules();
        ERR_load_crypto_strings();
        OpenSSL_add_all_ciphers();
        OpenSSL_add_all_algorithms();
        OpenSSL_add_all_digests();
    }
    const char *payload = env->GetStringUTFChars(payload_, 0);

    CX509Cert xcert;
    xcert.Create(1024, "Harry Bot", "751302230@qq.com", "secret");
    std::string sig = xcert.Sign(payload);

    env->ReleaseStringUTFChars(payload_, payload);

    return env->NewStringUTF(sig.c_str());
}
