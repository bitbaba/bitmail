#!/bin/bash
BitmailRepoRoot=/d/workspace/github/bitmail
QMake=/c/Qt/Qt5.7.0/5.7/mingw53_32/bin/qmake
#QMake=/opt/Qt5.8.0/5.8/gcc_64/bin/qmake
MAKE=make
#QtSpec="linux-g++-64" 
QtSpec="win32-g++" 
#QtSpec="macx-g++"

BITMAIL_QT_ROOT=${BitmailRepoRoot}/qt
BITMAIL_QT_BUILD_ROOT=${BitmailRepoRoot}/out/qt

cd ${BITMAIL_QT_ROOT} && \
sh ./vergen.sh 

mkdir -p ${BITMAIL_QT_BUILD_ROOT} ; \
cd ${BITMAIL_QT_BUILD_ROOT} && \
${QMake} -spec $QtSpec ${BITMAIL_QT_ROOT}/bitmail.pro && \
${MAKE} clean && \
${MAKE} 

mkdir -p ${BITMAIL_QT_BUILD_ROOT}/ext/audiorecorder ; \
cd ${BITMAIL_QT_BUILD_ROOT}/ext/audiorecorder  && \
${QMake} -spec $QtSpec ${BITMAIL_QT_ROOT}/ext/audiorecorder/audiorecorder.pro && \
${MAKE} clean && \
${MAKE} 

mkdir -p ${BITMAIL_QT_BUILD_ROOT}/ext/camera ; \
cd ${BITMAIL_QT_BUILD_ROOT}/ext/camera  && \
${QMake} -spec $QtSpec ${BITMAIL_QT_ROOT}/ext/camera/camera.pro && \
${MAKE} clean && \
${MAKE} 

mkdir -p ${BITMAIL_QT_BUILD_ROOT}/ext/screenshot ; \
cd ${BITMAIL_QT_BUILD_ROOT}/ext/screenshot  && \
${QMake} -spec $QtSpec ${BITMAIL_QT_ROOT}/ext/screenshot/screenshot.pro && \
${MAKE} clean && \
${MAKE} 