AM_CPPFLAGS=-I$(top_srcdir)/include

lib_LTLIBRARIES=libbitmailcore.la

libbitmailcore_la_SOURCES= \
    bitmail.cpp \
    x509cert.cpp \
    email.cpp \
	brad.cpp \
	rpc.cpp 

libbitmailcore_la_CXXFLAGS  = -DCURL_STATICLIB -DMINIUPNP_STATICLIB
libbitmailcore_la_LDFLAGS   = -no-undefined -export-all-symbols -compat-implib #-out-implib libbitmailcore.dll.a
libbitmailcore_la_LIBADD    = -lcurl -lssl -lcrypto -lmicrohttpd -lminiupnpc
