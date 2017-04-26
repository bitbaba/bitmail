QT += multimedia widgets

win32:INCLUDEPATH += $$PWD

HEADERS = \
    audiorecorder.h \
    qaudiolevel.h

SOURCES = \
    main.cpp \
    audiorecorder.cpp \
    qaudiolevel.cpp

FORMS += audiorecorder.ui

