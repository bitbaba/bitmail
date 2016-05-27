#!/bin/bash
BitmailCoreHome="$(dirname $(readlink -f "$0") )"
BitmailHome="$(dirname $BitmailCoreHome)"
BitmailOutHome=$BitmailHome/out

cd $BitmailCoreHome

g++ -I$BitmailOutHome/include \
	 -I$BitmailCoreHome/include \
	 -o $BitmailOutHome/lib/libbitmailapi.so \
	 -shared -fPIC -fpic -g -O2 \
	 src/bitmail_api.cpp \
	 -L$BitmailOutHome/lib \
	 -lbitmailcore -lcurl -lssl -lcrypto -lgdi32 -lws2_32

