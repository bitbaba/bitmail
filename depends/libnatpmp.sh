#!/bin/bash
DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="libnatpmp-20150609.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/libnatpmp-20150609"
DOWNLINK="http://miniupnp.free.fr/files/libnatpmp-20150609.tar.gz"
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
	cp -f Makefile.natpmp "$DESTDIR"/Makefile
	cp -f natpmp.getgateway.c "$DESTDIR"/getgateway.c
	cp -f natpmp.h "$DESTDIR"/natpmp.h
	cd "$DESTDIR"
	make DESTDIR="$OutDir" INSTALLPREFIX="" install && make clean
fi
