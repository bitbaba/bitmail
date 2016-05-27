#!/bin/bash
DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

OPENSSLTARBALL=openssl-1.0.2h.tar.gz
CURLTARBALL=curl-7.48.0.tar.gz

cd $DepSrcRoot

if [ -f openssl-1.0.2h.tar.gz ]; then
	echo "Use local cache of openssl "
else
	wget --no-check-certificate https://www.openssl.org/source/openssl-1.0.2h.tar.gz
fi

tar zxvf openssl-1.0.2h.tar.gz -C /tmp
cd /tmp/openssl-1.0.2h
./config -fPIC \
    no-asm \
    no-zlib \
    no-shared \
    no-dso \
    --prefix=$OutDir && make && make install && make clean

cd $DepSrcRoot

if [ -f curl-7.48.0.tar.gz ]; then
	echo "Use local cache of curl "
else
	wget --no-check-certificate https://curl.haxx.se/download/curl-7.48.0.tar.gz
fi

tar zxvf curl-7.48.0.tar.gz -C /tmp
cd /tmp/curl-7.48.0
./configure --prefix=$OutDir \
            --with-ssl=$OutDir \
            --disable-shared \
            --enable-static \
            --disable-ldap \
            --disable-ldaps \
           --without-zlib && make && make install && make clean



