QT += widgets network

ICON = res/image/bitmail.icns

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
                rssdialog.h \
                walletdialog.h \
                paydialog.h \
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
                rssdialog.cpp \
                walletdialog.cpp \
                paydialog.cpp \
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
    rssdialog.ui \
    walletdialog.ui \
    paydialog.ui \
    versiondialog.ui

# library: bitmailcore
unix|win32: LIBS += -L$$PWD/../out/core/lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../out/core/include
DEPENDPATH += $$PWD/../out/core/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/core/lib/bitmailcore.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/core/lib/libbitmailcore.a

# library: curl
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -lcurl

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/curl.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/libcurl.a

# library: openssl
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -lssl

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/ssl.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/libssl.a

# library: crypto
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -lcrypto

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/crypto.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/libcrypto.a

# library: microhttpd
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -lmicrohttpd

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/microhttpd.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/libmicrohttpd.a

# library: qrencode
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -lqrencode

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/qrencode.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/libqrencode.a

# library: lua
unix|win32: LIBS += -L$$PWD/../out/depends/lib/ -llua

INCLUDEPATH += $$PWD/../out/depends/include
DEPENDPATH += $$PWD/../out/depends/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/lua.lib
else:unix|win32-g++: PRE_TARGETDEPS += $$PWD/../out/depends/lib/liblua.a


win32: LIBS += -lws2_32 -lgdi32 -liphlpapi

DISTFILES += \
    bitmail.rc
