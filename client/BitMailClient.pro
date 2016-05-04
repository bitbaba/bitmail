TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/bitmailclient.cpp

QMAKE_CFLAGS = -Wno-unused-parameter -DCURL_STATICLIB

include(deployment.pri)

qtcAddDeployment()

#libbitmailcore
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../lib/ -lbitmailcore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lbitmailcored
else:unix: LIBS += -L$$PWD/../lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libbitmailcored.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/bitmailcore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/bitmailcored.lib
else:unix: PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a

### libjsoncpp
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rd/jsoncpp-0.10.2-p1/lib/ -ljsoncpp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rd/jsoncpp-0.10.2-p1/lib/ -ljsoncppd
else:unix: LIBS += -L$$PWD/../3rd/jsoncpp-0.10.2-p1/lib/ -ljsoncpp

INCLUDEPATH += $$PWD/../3rd/jsoncpp-0.10.2-p1/include
DEPENDPATH += $$PWD/../3rd/jsoncpp-0.10.2-p1/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/jsoncpp-0.10.2-p1/lib/libjsoncpp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/jsoncpp-0.10.2-p1/lib/libjsoncppd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/jsoncpp-0.10.2-p1/lib/jsoncpp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/jsoncpp-0.10.2-p1/lib/jsoncppd.lib
else:unix: PRE_TARGETDEPS += $$PWD/../3rd/jsoncpp-0.10.2-p1/lib/libjsoncpp.a

### libcurl
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rd/curl-7.42/lib/ -lcurl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rd/curl-7.42/lib/ -lcurld
else:unix: LIBS += -L$$PWD/../3rd/curl-7.42/lib/ -lcurl

INCLUDEPATH += $$PWD/../3rd/curl-7.42/include
DEPENDPATH += $$PWD/../3rd/curl-7.42/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/curl-7.42/lib/libcurl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/curl-7.42/lib/libcurld.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/curl-7.42/lib/curl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/curl-7.42/lib/curld.lib
else:unix: PRE_TARGETDEPS += $$PWD/../3rd/curl-7.42/lib/libcurl.a

### libssl
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lssl
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lssld
else:unix: LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lssl

INCLUDEPATH += $$PWD/../3rd/openssl-1.0.2a/include
DEPENDPATH += $$PWD/../3rd/openssl-1.0.2a/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libssl.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libssld.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/ssl.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/ssld.lib
else:unix: PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libssl.a

### libcrypto
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lcrypto
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lcryptod
else:unix: LIBS += -L$$PWD/../3rd/openssl-1.0.2a/lib/ -lcrypto

INCLUDEPATH += $$PWD/../3rd/openssl-1.0.2a/include
DEPENDPATH += $$PWD/../3rd/openssl-1.0.2a/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libcrypto.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libcryptod.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/crypto.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/cryptod.lib
else:unix: PRE_TARGETDEPS += $$PWD/../3rd/openssl-1.0.2a/lib/libcrypto.a

### for win32 system libs
win32: LIBS += -lgdi32 -lws2_32

