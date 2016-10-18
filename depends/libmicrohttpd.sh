#!/bin/bash
if [ x$(uname -s) == x"Darwin" ]; then
	alias readlink='greadlink'
fi

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

if [[ $(uname -s) != "MINGW"* ]]; then
	PICFLAGS='-fPIC -fpic'
fi


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
