#!/bin/bash

#mingw-get.exe install msys-unzip
NSIS_VERSION="2.51"
NSIS_RELEASE="nsis-$NSIS_VERSION"
NSIS_BALL="nsis-$NSIS_VERSION.zip"
NSIS_CMD="$NSIS_RELEASE/makensis.exe"
NSIS_LINK="https://svwh.dl.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/$NSIS_BALL"
CURL_OPT="-k"
#CURL_OPT="-k --proxy socks5://127.0.0.1:1080/"

BITMAIL_QT_ROOT="D:\\workspace\\github\\bitmail\\qt"
BITMAIL_DEPENDS_ROOT="D:\\workspace\\github\\bitmail\\depends"
BITMAIL_CORE_ROOT="D:\\workspace\\github\\bitmail\\core"
BITMAIL_QT_BUILD_ROOT="$BITMAIL_QT_ROOT\\build"
BITMAIL_QT_RELEASE_ROOT="$BITMAIL_QT_BUILD_ROOT\\release"
BITMAIL_QT_INSTALLER_ROOT="$BITMAIL_QT_ROOT\\win32-installer"

QT_LICENSE_ROOT="C:\\Qt\\Qt5.7.0\\Licenses"
QT_DIST_ROOT="C:\\Qt\\Qt5.7.0\\5.7\\mingw53_32\\bin"
QT_DIST_ICU_ROOT="C:\\Qt\\Qt5.7.0\\Tools\\QtCreator\\bin"
QT_TOOLS_ROOT="C:\\Qt\\Qt5.7.0\\Tools\\mingw530_32\\bin"
QT_PLUGINS_ROOT="C:\\Qt\\Qt5.7.0\\5.7\mingw53_32\\plugins"

#QT_LICENSE_ROOT="C:\\Qt\\Qt5.4.1\\Licenses"
#QT_DIST_ROOT="C:\\Qt\\Qt5.4.1\\5.4\\mingw491_32\\bin"
#QT_DIST_ICU_ROOT="C:\\Qt\\Qt5.4.1\\5.4\\mingw491_32\\bin"
#QT_TOOLS_ROOT="C:\\Qt\\Qt5.4.1\\Tools\\mingw530_32\\bin"
#QT_PLUGINS_ROOT="C:\\Qt\\Qt5.4.1\\5.4\mingw491_32\\plugins"

cd $BITMAIL_DEPENDS_ROOT
./openssl.sh && ./cur.sh && ./libmicrohttpd.sh && ./miniupnpc.sh && ./qrencode.sh

cd $BITMAIL_CORE_ROOT
./autogen.sh

cd $BITMAIL_QT_ROOT
./vergen.sh 

cd $BITMAIL_QT_BUILD_ROOT
qmake -spec win32-g++ ../bitmail.pro
mingw32-make clean && mingw32-make 

cd $BITMAIL_QT_INSTALLER_ROOT
if ! [ -f "$NSIS_BALL" ]; then
	$BITMAIL_QT_ROOT/out/bin/curl $CURL_OPT -o $NSIS_BALL $NSIS_LINK
fi

if ! [ -f "$NSIS_CMD" ]; then
	unzip "$NSIS_BALL"
fi

if   [ -f "$NSIS_CMD" ]; then
	$NSIS_CMD /dBitMailQtRoot="$BITMAIL_QT_ROOT" \
			  /dBitMailQtReleaseRoot="$BITMAIL_QT_RELEASE_ROOT" \
			  /dQtLicenseRoot="$QT_LICENSE_ROOT" \
			  /dQtDistRoot="$QT_DIST_ROOT" \
			  /dQtDistIcuRoot="$QT_DIST_ICU_ROOT" \
			  /dQtToolsRoot="$QT_TOOLS_ROOT" \
			  /dQtPluginsRoot="$QT_PLUGINS_ROOT" \
			  setup.nsi
fi
