#!/bin/bash
BitmailCoreHome="$(dirname $(readlink -f "$0") )"
BitmailHome="$(dirname $BitmailCoreHome)"
BitmailOutHome=$BitmailHome/out

cd $BitmailCoreHome

aclocal -I m4 \
&& autoconf \
&& automake -fac \
&& ./configure --prefix="$BitmailOutHome" \
    --enable-static=yes  \
    --enable-shared=no \
    CXXFLAGS="-g -O2" \
    CPPFLAGS="-I$BitmailOutHome/include" \
    LDFLAGS=" -L$BitmailOutHome/lib" \
&& make && make install && make clean

echo g++ -I$BitmailOutHome/include \
	 -I$BitmailCoreHome/include \
	 -o $BitmailOutHome/lib/libbitmailapi.so \
	 -shared -fPIC -fpic -g -O2 \
	 src/bitmail_api.cpp \
	 -L$BitmailOutHome/lib \
	 -lbitmailcore

g++ -I$BitmailOutHome/include \
	 -I$BitmailCoreHome/include \
	 -o $BitmailOutHome/lib/libbitmailapi.so \
	 -shared -fPIC -fpic -g -O2 \
	 src/bitmail_api.cpp \
	 -L$BitmailOutHome/lib \
	 -lbitmailcore

