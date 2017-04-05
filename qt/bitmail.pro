QT += widgets network multimedia

CODECFORTR = utf-8
TRANSLATIONS = locale/bitmail_en.ts \
			   locale/bitmail_zh_CN.ts \
			   locale/bitmail_zh_TW.ts \
			   locale/bitmail_ja.ts \
			   locale/bitmail_ru.ts \
			   locale/bitmail_fr.ts \
			   locale/bitmail_de.ts 

HEADERS       = mainwindow.h \
                optiondialog.h \
                logindialog.h \
                main.h \
                txthread.h \
                rxthread.h \
                msgqueue.h \
                certdialog.h \
                shutdowndialog.h \
                invitedialog.h \
                messagedialog.h \
                netoptdialog.h \
                assistantdialog.h \
                lock.h \
    ardialog.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                optiondialog.cpp \
                logindialog.cpp \
                txthread.cpp \
                rxthread.cpp \
                certdialog.cpp \
                shutdowndialog.cpp \
                invitedialog.cpp \
                messagedialog.cpp \
                netoptdialog.cpp \
                assistantdialog.cpp \
                lock.cpp \
    ardialog.cpp
#! [0]
RESOURCES     = bitmail.qrc
#! [0]

win32:RC_FILE = bitmail.rc

FORMS += \
    optiondialog.ui \
    logindialog.ui \
    certdialog.ui \
    shutdowndialog.ui \
    invitedialog.ui \
    messagedialog.ui \
    netoptdialog.ui \
    assistantdialog.ui \
    ardialog.ui

win32: LIBS += -L$$PWD/../out/lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/bitmailcore.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libbitmailcore.a

win32: LIBS += -L$$PWD/../out/lib/ -lcurl

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/curl.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libcurl.a

win32: LIBS += -L$$PWD/../out/lib/ -lssl

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/ssl.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libssl.a

win32: LIBS += -L$$PWD/../out/lib/ -lcrypto

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/crypto.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libcrypto.a

win32: LIBS += -L$$PWD/../out/lib/ -lmicrohttpd

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/microhttpd.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libmicrohttpd.a

win32: LIBS += -L$$PWD/../out/lib/ -lminiupnpc

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/miniupnpc.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libminiupnpc.a


win32: LIBS += -L$$PWD/../out/lib/ -lqrencode

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/qrencode.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libqrencode.a


win32: LIBS += -lws2_32 -lgdi32 -liphlpapi

DISTFILES += \
    bitmail.rc

