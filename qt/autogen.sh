#!/bin/bash

QMake=/opt/Qt5.8.0/5.8/gcc_64/bin/qmake
MAKE=make
QtSpec="linux-g++-64"

BitmailRepoRoot=~/workspace/bitmail
BITMAIL_QT_ROOT="$BitmailRepoRoot/qt"
BITMAIL_DEPENDS_ROOT="$BitmailRepoRoot/depends"
BITMAIL_CORE_ROOT="$BitmailRepoRoot/core"
BITMAIL_QT_BUILD_ROOT="$BITMAIL_QT_ROOT/build"

cd $BITMAIL_DEPENDS_ROOT
false && sh ./openssl.sh && \
sh ./curl.sh && \
sh ./libmicrohttpd.sh && \
sh ./miniupnpc.sh && \
sh ./qrencode.sh && \
sh ./lua.sh

cd $BITMAIL_CORE_ROOT
sh ./autogen.sh

cd $BITMAIL_QT_ROOT
sh ./vergen.sh 

cd $BITMAIL_QT_BUILD_ROOT
$QMake -spec $QtSpec ../bitmail.pro
$MAKE clean && $MAKE 

