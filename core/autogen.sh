#!/bin/bash
BitmailCoreHome=${PWD}
BitmailCoreDependsHome=${BitmailCoreHome}/../out/depends
BitmailCoreOutHome=${BitmailCoreHome}/../out/core

# Version
echo \#define BITMAIL_CORE_GIT_VERSION \"$(git log -1 .| grep commit | cut -d " " -f 2)\" > $BitmailCoreHome/include/bitmailcore/version.h

aclocal -I m4 \
&& autoconf \
&& automake -fac \
&& ./configure --prefix="$BitmailCoreOutHome" \
    --enable-static=yes  \
    --enable-shared=no \
    CXXFLAGS="-fPIC -fpic" \
    CPPFLAGS="-I$BitmailCoreDependsHome/include" \
    LDFLAGS=" -L$BitmailCoreDependsHome/lib" \
&& make \
&& make install \
&& make clean


