#!/bin/bash

QtBinRoot=/opt/Qt5.8.0/5.8/gcc_64/bin

QtLibRoot=/opt/Qt5.8.0/5.8/gcc_64/lib

BitmailRepoRoot=${PWD}/../../..

BitmailQtRoot=${BitmailRepoRoot}/qt

BitmailQtBuildRoot=${BitmailRepoRoot}/out/qt

SandBox=${PWD}/sandbox

mkdir -p ${SandBox}

COPY="cp -rfL"

${COPY} ${BitmailQtBuildRoot}/bitmail   ${SandBox}
${COPY} ${BitmailQtBuildRoot}/audiorecorder ${SandBox}
${COPY} ${BitmailQtBuildRoot}/camera   ${SandBox}
${COPY} ${BitmailQtBuildRoot}/screenshot   ${SandBox}
${COPY} ${QtLibRoot}/libQt5Widgets.so.5 ${SandBox}
${COPY} ${QtLibRoot}/libQt5Gui.so.5     ${SandBox}
${COPY} ${QtLibRoot}/libQt5Network.so.5 ${SandBox}
${COPY} ${QtLibRoot}/libQt5Core.so.5    ${SandBox}
${COPY} ${QtLibRoot}/libicui18n.so.56   ${SandBox}
${COPY} ${QtLibRoot}/libicuuc.so.56     ${SandBox}
${COPY} ${QtLibRoot}/libicudata.so.56   ${SandBox}
${COPY} ${BitmailQtRoot}/res   ${SandBox}
mkdir -p ${SandBox}/locale
${COPY} ${BitmailQtRoot}/locale/*.qm   ${SandBox}/locale
