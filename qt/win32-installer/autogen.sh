#!/bin/bash

NSIS_BALL="nsis-2.30.zip"
#NSIS_Cmd="nsis-2.30/makensis.exe"
NSIS_Cmd="nsis/makensis.exe"

#mingw-get.exe install msys-unzip

if ! [ -f "$NSIS_BALL" ]; then
../../out/bin/curl -k --proxy socks5://127.0.0.1:1080/ -o nsis-2.30.zip https://svwh.dl.sourceforge.net/project/nsis/NSIS%202/2.30/nsis-2.30.zip
fi

if ! [ -f "$NSIS_Cmd" ]; then
unzip "$NSIS_BALL"
fi

if  [ -f "$NSIS_Cmd" ]; then
$NSIS_Cmd /dBitMailQtRoot="D:\\workspace\\github\\bitmail\\qt" \
          /dBitMailQtReleaseRoot="D:\\workspace\\github\\bitmail\\qt\\release" \
          /dQtDistRoot="C:\\Qt\\Qt5.7.0" \
          /dQtDistPluginsRoot="C:\\Qt\\Qt5.7.0\\5.7\\mingw53_32\\plugins" \
		  setup.nsi
fi
