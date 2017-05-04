#!/bin/bash
if [ x$(uname -s) = x'Darwin' ]; then
	alias readlink='greadlink'
fi

BitmailCoreHome="$(dirname $(readlink -f "$0") )"
BitmailHome="$(dirname $BitmailCoreHome)"
BitmailOutHome=$BitmailHome/out

cd $BitmailCoreHome

if [[ $(uname -s) != "MINGW"* ]]; then
	PICFLAGS='-fPIC -fpic'
fi

# Version
echo \#define BITMAIL_CORE_GIT_VERSION \"$(git log -1 | grep commit | cut -d " " -f 2)\" > $BitmailCoreHome/include/bitmailcore/version.h

DebugFlags="-g -O0"

aclocal -I m4 \
&& autoconf \
&& automake -fac \
&& ./configure --prefix="$BitmailOutHome" \
    --enable-static=yes  \
    --enable-shared=no \
    CXXFLAGS="${DebugFlags} ${PICFLAGS}" \
    CPPFLAGS="-I$BitmailOutHome/include" \
    LDFLAGS=" -L$BitmailOutHome/lib" \
&& make && make install && make clean


