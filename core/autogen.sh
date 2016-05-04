#!/bin/bash
BitmailCoreHome="$(dirname $(readlink -f "$0") )"
BitmailHome="$(dirname $BitmailCoreHome)"
BitmailOutHome=$BitmailHome/out

cd $BitmailCoreHome

aclocal -I m4 \
&& autoconf \
&& automake -fac \
&& ./configure --prefix="$BitmailOutHome" \
    --enable-static=no  \
    --enable-shared=yes \
    CXXFLAGS="-g -O2" \
    CPPFLAGS="-I$BitmailOutHome/include" \
    LDFLAGS=" -L$BitmailOutHome/lib" \
&& make && make install && make clean
