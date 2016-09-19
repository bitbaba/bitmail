#!/bin/bash
DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="libmicrohttpd-0.9.51.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/libmicrohttpd-0.9.51"
DOWNLINK="http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.51.tar.gz"
DOWNOPTS=""

if ! [ -f "$TARBALL" ]; then
	echo "Downloading... $DOWNLINK"
	wget "$DOWNOPTS" "$DOWNLINK"
fi

if [ -f "$TARBALL" ]; then
	echo "Decompressing... $TARBALL "
	tar "$TAROPTS" "$TARBALL" -C "$TARDEST"
fi

if [ -d "$DESTDIR" ]; then
	cd "$DESTDIR"
    ./configure   --prefix="$OutDir" \
                    --enable-shared=no \
                    --enable-static=yes \
                    --enable-https=no \
                    --enable-bauth \
                    --enable-dauth \
                    --disable-curl \
                    --disable-doc \
                    --disable-examples \
                    --disable-poll \
                    --disable-epoll && make && make install && make clean
fi

#==================================================
cd $DepSrcRoot
TARBALL="openssl-1.0.2h.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/openssl-1.0.2h"
DOWNLINK="https://www.openssl.org/source/openssl-1.0.2h.tar.gz"
DOWNOPTS="--no-check-certificate"

if ! [ -f "$TARBALL" ]; then
	echo "Downloading... $DOWNLINK"
	wget "$DOWNOPTS" "$DOWNLINK"
fi

if [ -f "$TARBALL" ]; then
	echo "Decompressing... $TARBALL "
	tar "$TAROPTS" "$TARBALL" -C "$TARDEST"
fi

if [ -d "$DESTDIR" ]; then
	cd "$DESTDIR"
	./config -fPIC \
			no-asm \
			no-zlib \
			no-shared \
			no-dso \
			--prefix="$OutDir" && make && make install && make clean
fi

#==================================================
cd $DepSrcRoot
TARBALL="curl-7.48.0.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/curl-7.48.0"
DOWNLINK="https://curl.haxx.se/download/curl-7.48.0.tar.gz"
DOWNOPTS="--no-check-certificate"

if ! [ -f "$TARBALL" ]; then
	echo "Downloading... $DOWNLINK"
	wget "$DOWNOPTS" "$DOWNLINK"
fi

if [ -f "$TARBALL" ]; then
	echo "Decompressing... $TARBALL "
	tar "$TAROPTS" "$TARBALL" -C "$TARDEST"
fi

if [ -d "$DESTDIR" ]; then
	cd "$DESTDIR"
	./configure --prefix="$OutDir" \
				--with-ssl="$OutDir" \
				--disable-shared \
				--enable-static \
				--disable-ldap \
				--disable-ldaps \
			   --without-zlib && make && make install && make clean
fi
