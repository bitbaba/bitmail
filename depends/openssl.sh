#!/bin/bash
if [ x$(uname -s) == x"Darwin" ]; then
	alias readlink='greadlink'
fi

DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

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

