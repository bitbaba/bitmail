#!/bin/bash
if [ x$(uname -s) == x"Darwin" ]; then
	alias readlink='greadlink'
fi

DepSrcRoot=$(dirname $(readlink -f $0) )
OutDir=$DepSrcRoot/../out

#==================================================
cd $DepSrcRoot
TARBALL="miniupnpc-1.9.20160209.tar.gz"
TAROPTS=zxvf
TARDEST="/tmp"
DESTDIR="/tmp/miniupnpc-1.9.20160209"
DOWNLINK="http://miniupnp.free.fr/files/miniupnpc-1.9.20160209.tar.gz"
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
	case $(uname -s) in 
		Linux*|Darwin*)
			make -f Makefile INSTALLPREFIX="$OutDir" install-static
		;;
		MINGW32*)
			cp -f $DepSrcRoot/miniupnpc.Makefile.mingw $DESTDIR/Makefile.mingw
			make -f Makefile.mingw INSTALLPREFIX="$OutDir" install-static
		;;
		*)
			echo "Unknown platform"
		;;
	esac
fi
