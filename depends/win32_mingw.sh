#!/bin/bash
export INSTALLROOT=${PWD}/../out/win32_mingw
export OPENSSL_CFLAGS=
export OPENSSL_PLATFORM=mingw
export LUA_PLATFORM=generic

MAKE=mingw32-make

${MAKE} -f openssl.mk 

${MAKE} -f curl.mk 

${MAKE} -f lua.mk 

${MAKE} -f microhttpd.mk 

${MAKE} -f qrencode.mk 
