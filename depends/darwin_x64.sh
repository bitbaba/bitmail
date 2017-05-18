#!/bin/bash
export INSTALLROOT=${PWD}/../out/darwin_x64
export OPENSSL_CFLAGS="-fpic -fPIC"
export OPENSSL_PLATFORM=darwin64-x86_64-cc
export LUA_PLATFORM=macosx

MAKE=make

${MAKE} -f openssl.mk 

${MAKE} -f curl.mk 

${MAKE} -f lua.mk 

${MAKE} -f microhttpd.mk 

${MAKE} -f qrencode.mk 

ln -s $INSTALLROOT ${INSTALLROOT%/*}/depends
