#!/bin/bash

NSIS_VERSION="2.51"
NSIS_RELEASE="nsis-$NSIS_VERSION"
NSIS_BALL="nsis-$NSIS_VERSION.zip"
NSIS_CMD="$NSIS_RELEASE/makensis.exe"
NSIS_LINK="https://svwh.dl.sourceforge.net/project/nsis/NSIS%202/$NSIS_VERSION/$NSIS_BALL"

BITMAIL_QT_ROOT="D:\\workspace\\github\\bitmail\\qt"
BITMAIL_QT_RELEASE_ROOT="$BITMAIL_QT_ROOT\\..\\out\\qt\\release"

QT_LICENSE_ROOT="C:\\Qt\\Qt5.7.0\\Licenses"
QT_DIST_ROOT="C:\\Qt\\Qt5.7.0\\5.7\\mingw53_32\\bin"
QT_DIST_ICU_ROOT="C:\\Qt\\Qt5.7.0\\Tools\\QtCreator\\bin"
QT_TOOLS_ROOT="C:\\Qt\\Qt5.7.0\\Tools\\mingw530_32\\bin"
QT_PLUGINS_ROOT="C:\\Qt\\Qt5.7.0\\5.7\mingw53_32\\plugins"

if ! [ -f ${NSIS_BALL} ]; then
	curl -k ${NSIS_LINK} -o ${NSIS_BALL}
fi

if ! [ -f ${NSIS_CMD} ]; then
	unzip ${NSIS_BALL}
fi

if   [ -f ${NSIS_CMD} ]; then
	${NSIS_CMD} /dBitMailQtRoot="$BITMAIL_QT_ROOT" \
			  /dBitMailQtReleaseRoot="$BITMAIL_QT_RELEASE_ROOT" \
			  /dQtLicenseRoot="$QT_LICENSE_ROOT" \
			  /dQtDistRoot="$QT_DIST_ROOT" \
			  /dQtDistIcuRoot="$QT_DIST_ICU_ROOT" \
			  /dQtToolsRoot="$QT_TOOLS_ROOT" \
			  /dQtPluginsRoot="$QT_PLUGINS_ROOT" \
			  setup.nsi
fi
