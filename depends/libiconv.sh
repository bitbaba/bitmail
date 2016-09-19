#!/bin/bash
DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="libiconv-1.14.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/libiconv-1.14"
DOWNLINK="http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz"
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
                    --enable-extra-encodings && make && make install && make clean
fi
