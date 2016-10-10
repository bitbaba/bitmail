QT += widgets network

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
                pollthread.h \
                paydialog.h \
                certdialog.h \
                shutdowndialog.h \
                invitedialog.h \
                messagedialog.h \
                walletdialog.h \
                rssdialog.h \
                netoptdialog.h \
                assistantdialog.h \
                newgroupdialog.h \
    upnpthread.h \
    bradthread.h \
    bracthread.h

SOURCES       = main.cpp \
                mainwindow.cpp \
                optiondialog.cpp \
                logindialog.cpp \
                txthread.cpp \
                rxthread.cpp \
                msgqueue.cpp \
                pollthread.cpp \
                paydialog.cpp \
                certdialog.cpp \
                shutdowndialog.cpp \
                invitedialog.cpp \
                messagedialog.cpp \
                walletdialog.cpp \
                rssdialog.cpp \
                netoptdialog.cpp \
                assistantdialog.cpp \
                newgroupdialog.cpp \
    upnpthread.cpp \
    bradthread.cpp \
    bracthread.cpp
#! [0]
RESOURCES     = bitmail.qrc
#! [0]

win32:RC_FILE = bitmail.rc

FORMS += \
    optiondialog.ui \
    logindialog.ui \
    paydialog.ui \
    certdialog.ui \
    shutdowndialog.ui \
    invitedialog.ui \
    messagedialog.ui \
    walletdialog.ui \
    rssdialog.ui \
    netoptdialog.ui \
    assistantdialog.ui \
    newgroupdialog.ui

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


win32: LIBS += -L$$PWD/../out/lib/ -lminiupnpc

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/miniupnpc.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libminiupnpc.a


win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/microhttpd.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libmicrohttpd.a

win32: LIBS += -lws2_32 -lgdi32 -liphlpapi

DISTFILES += \
    bitmail.rc

