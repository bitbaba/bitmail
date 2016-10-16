#!/bin/bash
if [ x$(uname -s) == x"Darwin" ]; then
	alias readlink='greadlink'
fi

DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

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
