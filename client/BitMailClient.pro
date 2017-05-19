CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/bitmailclient.cpp

QMAKE_CFLAGS = -Wno-unused-parameter -DCURL_STATICLIB

CODECFORTR = utf-8

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


