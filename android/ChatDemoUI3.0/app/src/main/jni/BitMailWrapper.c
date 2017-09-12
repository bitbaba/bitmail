#include <jni.h>


JNIEXPORT jstring JNICALL
Java_com_hyphenate_chatuidemo_ui_MyEaseChatFragment_NativeEncMessage(JNIEnv *env, jobject instance,
                                                                     jstring signer_,
                                                                     jstring buddy_,
                                                                     jstring text_) {
    const char *signer = (*env)->GetStringUTFChars(env, signer_, 0);
    const char *buddy = (*env)->GetStringUTFChars(env, buddy_, 0);
    const char *text = (*env)->GetStringUTFChars(env, text_, 0);

    // TODO

    (*env)->ReleaseStringUTFChars(env, signer_, signer);
    (*env)->ReleaseStringUTFChars(env, buddy_, buddy);
    (*env)->ReleaseStringUTFChars(env, text_, text);

    return (*env)->NewStringUTF(env, "returnValue");
}