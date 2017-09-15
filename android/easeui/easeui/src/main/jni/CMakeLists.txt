# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# for openssl
# ref: https://wiki.openssl.org/images/7/70/Setenv-android.sh
#
include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/include/${ANDROID_ABI})

add_library(Crypto STATIC IMPORTED )

add_library(Ssl    STATIC IMPORTED )

set_target_properties(Crypto PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/${ANDROID_ABI}/libcrypto.a)

set_target_properties(Ssl    PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/${ANDROID_ABI}/libssl.a)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             BitMailWrapper

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             ${PROJECT_SOURCE_DIR}/BitMailWrapper.cpp
             ${PROJECT_SOURCE_DIR}/x509cert.cpp
             ${PROJECT_SOURCE_DIR}/jsoncpp/json_reader.cpp
             ${PROJECT_SOURCE_DIR}/jsoncpp/json_value.cpp
             ${PROJECT_SOURCE_DIR}/jsoncpp/json_writer.cpp
             )

find_library(nativeLogger log)

target_link_libraries(BitMailWrapper Crypto Ssl ${nativeLogger})