#!/bin/bash
if [ x$(uname -s) = x"Darwin" ]; then
	alias readlink='greadlink'
fi

#DepSrcRoot=$(dirname $(readlink -f $0) )
DepSrcRoot=~/workspace/github/bitmail/depends #$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="qrencode-3.4.4.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/qrencode-3.4.4"
DOWNLINK="https://fukuchi.org/works/qrencode/qrencode-3.4.4.tar.gz"
DOWNOPTS="--no-check-certificate"

if [[ $(uname -s) != "MINGW"* ]]; then
	PICFLAGS='-fPIC -fpic'
fi

if ! [ -f "$TARBALL" ]; then
	echo "Downloading... $DOWNLINK"
	# Note: wget in msys may not work in special work!
	#wget "$DOWNOPTS" "$DOWNLINK"
	curl -O "$DOWNLINK"
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
                    --without-tools && make && make install && make clean
fi
