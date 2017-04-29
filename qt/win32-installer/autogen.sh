#!/bin/bash

NSIS_VERSION="2.51"
NSIS_RELEASE="nsis-$NSIS_VERSION"
NSIS_BALL="nsis-$NSIS_VERSION.zip"
NSIS_CMD="$NSIS_RELEASE/makensis.exe"
NSIS_LINK="https://svwh.dl.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/$NSIS_BALL"
CURL_OPT="-k"
#CURL_OPT="-k --proxy socks5://127.0.0.1:1080/"

#BITMAIL_QT_ROOT="D:\\workspace\\github\\bitmail\\qt"
#BITMAIL_QT_RELEASE_ROOT="D:\\workspace\\github\\bitmail\\qt\\build\\release"

BITMAIL_QT_ROOT="D:\\github\\bitmail\\qt"
BITMAIL_QT_RELEASE_ROOT="D:\\github\\bitmail\\qt\\build\\release"

QT_LICENSE_ROOT="C:\\Qt\\Qt5.4.1\\Licenses"
QT_DIST_ROOT="C:\\Qt\\Qt5.4.1\\5.4\\mingw491_32\\bin"
QT_TOOLS_ROOT="C:\\Qt\\Qt5.4.1\\Tools\\mingw491_32\\bin"
QT_PLUGINS_ROOT="C:\\Qt\\Qt5.4.1\\5.4\mingw491_32\\plugins"
#NSIS_CMD="nsis/makensis.exe"

#mingw-get.exe install msys-unzip

if ! [ -f "$NSIS_BALL" ]; then
	../../out/bin/curl $CURL_OPT -o $NSIS_BALL $NSIS_LINK
fi

if ! [ -f "$NSIS_CMD" ]; then
	unzip "$NSIS_BALL"
fi

if   [ -f "$NSIS_CMD" ]; then
	$NSIS_CMD /dBitMailQtRoot="$BITMAIL_QT_ROOT" \
			  /dBitMailQtReleaseRoot="$BITMAIL_QT_RELEASE_ROOT" \
			  /dQtLicenseRoot="$QT_LICENSE_ROOT" \
			  /dQtDistRoot="$QT_DIST_ROOT" \
			  /dQtToolsRoot="$QT_TOOLS_ROOT" \
			  /dQtPluginsRoot="$QT_PLUGINS_ROOT" \
			  setup.nsi
fi
