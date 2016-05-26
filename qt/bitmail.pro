QT += widgets

HEADERS       = mainwindow.h \
    optiondialog.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                optiondialog.cpp
#! [0]
RESOURCES     = bitmail.qrc
#! [0]

win32:RC_FILE = bitmail.rc

FORMS += \
    optiondialog.ui
