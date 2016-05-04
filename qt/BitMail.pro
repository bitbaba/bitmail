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
    $$PWD/../include/bitmailcore/bitmail.h \
    $$PWD/../include/bitmailcore/cpop3receiver.h \
    $$PWD/../include/bitmailcore/csmtpsender.h \
    $$PWD/../include/bitmailcore/irx.hpp \
    $$PWD/../include/bitmailcore/itx.hpp \
    $$PWD/../include/bitmailcore/x509cert.h \
    $$PWD/../3rd/curl-7.42/include/curl/curl.h \
    $$PWD/../3rd/curl-7.42/include/curl/curlbuild.h \
    $$PWD/../3rd/curl-7.42/include/curl/curlrules.h \
    $$PWD/../3rd/curl-7.42/include/curl/curlver.h \
    $$PWD/../3rd/curl-7.42/include/curl/easy.h \
    $$PWD/../3rd/curl-7.42/include/curl/mprintf.h \
    $$PWD/../3rd/curl-7.42/include/curl/multi.h \
    $$PWD/../3rd/curl-7.42/include/curl/stdcheaders.h \
    $$PWD/../3rd/curl-7.42/include/curl/typecheck-gcc.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/assertions.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/autolink.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/config.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/features.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/forwards.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/json.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/reader.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/value.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/version.h \
    $$PWD/../3rd/jsoncpp-0.10.2-p1/include/json/writer.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/aes.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/asn1.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/asn1_mac.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/asn1t.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/bio.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/blowfish.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/bn.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/buffer.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/camellia.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/cast.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/cmac.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/cms.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/comp.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/conf.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/conf_api.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/crypto.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/des.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/des_old.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/dh.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/dsa.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/dso.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/dtls1.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/e_os2.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ebcdic.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ec.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ecdh.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ecdsa.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/engine.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/err.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/evp.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/hmac.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/idea.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/krb5_asn.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/kssl.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/lhash.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/md4.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/md5.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/mdc2.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/modes.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/obj_mac.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/objects.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ocsp.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/opensslconf.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/opensslv.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ossl_typ.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/pem.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/pem2.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/pkcs12.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/pkcs7.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/pqueue.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/rand.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/rc2.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/rc4.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ripemd.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/rsa.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/safestack.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/seed.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/sha.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/srp.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/srtp.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ssl.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ssl2.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ssl23.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ssl3.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/stack.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/symhacks.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/tls1.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ts.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/txt_db.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ui.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/ui_compat.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/whrlpool.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/x509.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/x509_vfy.h \
    $$PWD/../3rd/openssl-1.0.2a/include/openssl/x509v3.h

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
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../lib/ -lbitmailcored
else:unix: LIBS += -L$$PWD/../lib/ -lbitmailcore

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/libbitmailcored.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../lib/bitmailcore.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../lib/bitmailcored.lib
else:unix: PRE_TARGETDEPS += $$PWD/../lib/libbitmailcore.a

### for libcurl
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

### for libssl
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

### for libcrypto
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

### for libjsoncpp
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


### for win32 system libs
win32: LIBS += -lgdi32 -lws2_32

