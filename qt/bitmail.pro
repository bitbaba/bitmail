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
                certdialog.h \
                shutdowndialog.h \
                invitedialog.h \
                messagedialog.h \
                assistantdialog.h \
                groupdialog.h \
                procth.h \
                clickablelabel.h \
                textedit.h \
                lock.h \
                versiondialog.h \
                version.h

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
                assistantdialog.cpp \
                groupdialog.cpp \
                procth.cpp \
                clickablelabel.cpp \
                textedit.cpp \
                lock.cpp \
                versiondialog.cpp
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
    assistantdialog.ui \
    groupdialog.ui \
    versiondialog.ui

# library: bitmailcore
unix|win32: LIBS += -L$$PWD/../out/lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/bitmailcore.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libbitmailcore.a

# library: curl
unix|win32: LIBS += -L$$PWD/../out/lib/ -lcurl

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/curl.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libcurl.a

# library: openssl
unix|win32: LIBS += -L$$PWD/../out/lib/ -lssl

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/ssl.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libssl.a

# library: crypto
unix|win32: LIBS += -L$$PWD/../out/lib/ -lcrypto

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/crypto.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libcrypto.a

# library: microhttpd
unix|win32: LIBS += -L$$PWD/../out/lib/ -lmicrohttpd

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/microhttpd.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libmicrohttpd.a

# library: qrencode
unix|win32: LIBS += -L$$PWD/../out/lib/ -lqrencode

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/qrencode.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/libqrencode.a

# library: lua
unix|win32: LIBS += -L$$PWD/../out/lib/ -llua

INCLUDEPATH += $$PWD/../out/include
DEPENDPATH += $$PWD/../out/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/lua.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/lib/liblua.a


win32: LIBS += -lws2_32 -lgdi32 -liphlpapi

DISTFILES += \
    bitmail.rc
