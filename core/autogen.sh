#!/bin/bash
BitmailCoreHome="$(dirname $(readlink -f "$0") )"
BitmailHome="$(dirname $BitmailCoreHome)"
BitmailOutHome=$BitmailHome/out

cd $BitmailCoreHome

PICFLAGS=""

aclocal -I m4 \
&& autoconf \
&& automake -fac \
&& ./configure --prefix="$BitmailOutHome" \
    --enable-static=yes  \
    --enable-shared=no \
    CXXFLAGS="-g -O0 -fPIC -fpic " \
    CPPFLAGS="-I$BitmailOutHome/include" \
    LDFLAGS=" -L$BitmailOutHome/lib" \
&& make && make install && make clean


