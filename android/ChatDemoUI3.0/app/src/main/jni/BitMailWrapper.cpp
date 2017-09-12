//
// Created by Administrator on 9/11/2017.
//
#include <string>
#include <cstdint>
#include <android/log.h>
#include "BitMailWrapper.h"

BitMailInst::BitMailInst() {
    int x = 0;
    x = 1;
    std::string y = "hello, world";
    __android_log_print(ANDROID_LOG_ERROR, "BitMailWrapper", "Message: %s", "Hello");
}

BitMailInst::~BitMailInst() {

}