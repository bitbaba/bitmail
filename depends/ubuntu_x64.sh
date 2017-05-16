#!/bin/bash
export INSTALLROOT=${PWD}/../out/ubuntu_x64
export OPENSSL_CFLAGS="-fpic -fPIC"
export OPENSSL_PLATFORM=linux-x86_64
export LUA_PLATFORM=linux

MAKE=make

${MAKE} -f openssl.mk 

${MAKE} -f curl.mk 

${MAKE} -f lua.mk 

${MAKE} -f microhttpd.mk 

${MAKE} -f qrencode.mk 

ln -s $INSTALLROOT ${INSTALLROOT%/*}/depends
