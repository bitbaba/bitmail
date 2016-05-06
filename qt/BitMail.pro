TEMPLATE = app

QT += qml quick widgets

#QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter
QMAKE_CFLAGS = -Wno-unused-parameter -DCURL_STATICLIB

SOURCES += main.cpp \
    mainwindow.cpp \
    logindialog.cpp \
    createprofiledialog.cpp \
    serversetupdialog.cpp \
    threadgetmessage.cpp \
    dialogaddbuddy.cpp \
    dlgstrangerrequest.cpp \
    dlgshowqrcode.cpp

RESOURCES +=

RC_FILE = bitmail.rc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    mainwindow.h \
    logindialog.h \
    createprofiledialog.h \
    serversetupdialog.h \
    threadgetmessage.h \
    dialogaddbuddy.h \
    dlgstrangerrequest.h \
    dlgshowqrcode.h \
    $$PWD/../include/bitmailcore/bitmail.h 


FORMS += \
    mainwindow.ui \
    logindialog.ui \
    createprofiledialog.ui \
    serversetupdialog.ui \
    dialogaddbuddy.ui \
    dlgstrangerrequest.ui \
    dlgshowqrcode.ui

#### for libbitmailcore
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lbitmailcore
else:unix: LIBS += -L$$PWD/../lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../include
DEPENDPATH  += $$PWD/../include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a
else:unix: PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a


### for win32 system libs
win32: LIBS += -lgdi32 -lws2_32

