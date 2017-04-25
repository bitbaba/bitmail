#!/bin/bash

NSIS_VERSION="2.51"
NSIS_RELEASE="nsis-$NSIS_VERSION"
NSIS_BALL="nsis-$NSIS_VERSION.zip"
NSIS_CMD="$NSIS_RELEASE/makensis.exe"
NSIS_LINK="https://svwh.dl.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/$NSIS_BALL"
CURL_OPT="-k --proxy socks5://127.0.0.1:1080/"
#NSIS_CMD="nsis/makensis.exe"

#mingw-get.exe install msys-unzip

if ! [ -f "$NSIS_BALL" ]; then
	../../out/bin/curl $CURL_OPT -o $NSIS_BALL $NSIS_LINK
fi

if ! [ -f "$NSIS_CMD" ]; then
	unzip "$NSIS_BALL"
fi

if   [ -f "$NSIS_CMD" ]; then
	$NSIS_CMD /dBitMailQtRoot="D:\\workspace\\github\\bitmail\\qt" \
			  /dBitMailQtReleaseRoot="D:\\workspace\\github\\bitmail\\qt\\build\\release" \
			  /dQtDistRoot="C:\\Qt\\Qt5.7.0" \
			  /dQtDistPluginsRoot="C:\\Qt\\Qt5.7.0\\5.7\\mingw53_32\\plugins" \
			  setup.nsi
fi
