AM_CPPFLAGS=-I$(top_srcdir)/include -I$(top_srcdir)/src/jsoncpp/include

lib_LTLIBRARIES=libbitmailcore.la

libbitmailcore_la_SOURCES= \
    bitmail.cpp \
    x509cert.cpp \
    email.cpp \
	rpc.cpp \
    multipart.cpp \
    jsoncpp/src/json_reader.cpp \
    jsoncpp/src/json_value.cpp \
    jsoncpp/src/json_writer.cpp 

libbitmailcore_la_CXXFLAGS  = -DCURL_STATICLIB -DMINIUPNP_STATICLIB
libbitmailcore_la_LDFLAGS   = -no-undefined -export-all-symbols -compat-implib #-out-implib libbitmailcore.dll.a
libbitmailcore_la_LIBADD    = -lcurl -lssl -lcrypto -lmicrohttpd -lminiupnpc -lqrencode -llua
