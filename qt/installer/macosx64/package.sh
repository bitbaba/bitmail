#!/bin/bash
# http://www.embed-net.com/thread-121-1-1.html

QtBinRoot=/Users/harrywu/Qt5.8.0/5.8/clang_64/bin

QtLibRoot=/Users/harrywu/Qt5.8.0/5.8/clang_64/lib

BitmailRepoRoot=${PWD}/../../..

BitmailQtRoot=${BitmailRepoRoot}/qt

BitmailQtBuildRoot=${BitmailRepoRoot}/out/qt

package_name=bitmail-qt-macosx-x64

SandBox=${PWD}/${package_name}

mkdir -p ${SandBox}

BitmailBundle=${SandBox}/bitmail.app/Contents/MacOS


COPY="cp -RfL"

${COPY} ${BitmailQtBuildRoot}/bitmail.app   ${SandBox}
${COPY} ${BitmailQtBuildRoot}/audiorecorder.app/Contents/MacOS/audiorecorder ${BitmailBundle}
${COPY} ${BitmailQtBuildRoot}/camera.app/Contents/MacOS/camera   ${BitmailBundle}
${COPY} ${BitmailQtBuildRoot}/screenshot.app/Contents/MacOS/screenshot   ${BitmailBundle}
${COPY} ${QtLibRoot}/QtWidgets.framework ${BitmailBundle}
${COPY} ${QtLibRoot}/QtGui.framework     ${BitmailBundle}
${COPY} ${QtLibRoot}/QtNetwork.framework ${BitmailBundle}
${COPY} ${QtLibRoot}/QtCore.framework    ${BitmailBundle}
#${COPY} ${QtLibRoot}/icui18n.framework   ${BitmailBundle}
#${COPY} ${QtLibRoot}/icuuc.framework     ${BitmailBundle}
#${COPY} ${QtLibRoot}/icudata.framework   ${BitmailBundle}

${COPY} ${BitmailQtRoot}/res   ${BitmailBundle}

mkdir -p ${BitmailBundle}/locale
${COPY} ${BitmailQtRoot}/locale/*.qm   ${BitmailBundle}/locale

tar czf ${package_name}.tar.gz ${package_name}
