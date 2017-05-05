#!/bin/bash
if [ x$(uname -s) = x"Darwin" ]; then
	alias readlink='greadlink'
fi

DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="lua-5.3.4.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/lua-5.3.4"
DOWNLINK="http://www.lua.org/ftp/lua-5.3.4.tar.gz"
DOWNOPTS="--no-check-certificate"

if [[ $(uname -s) != "MINGW"* ]]; then
	PICFLAGS='-fPIC -fpic'
	PLAT='linux'
else
	PLAT='mingw'
fi

if ! [ -f "$TARBALL" ]; then
	echo "Downloading... $DOWNLINK"
	# Note: wget in msys may not work in special work!
	wget "$DOWNOPTS" "$DOWNLINK"
fi

if [ -f "$TARBALL" ]; then
	echo "Decompressing... $TARBALL "
	tar "$TAROPTS" "$TARBALL" -C "$TARDEST"
fi

if [ -d "$DESTDIR" ]; then
	cd "$DESTDIR"
    make $PLAT
	make install INSTALL_TOP=$OutDir
fi
