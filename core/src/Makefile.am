AM_CPPFLAGS=-I$(top_srcdir)/include

lib_LTLIBRARIES=libbitmailcore.la

libbitmailcore_la_SOURCES= \
    bitmail.cpp \
    x509cert.cpp \
    email.cpp 

libbitmailcore_la_LDFLAGS = -no-undefined
libbitmailcore_la_LIBADD = -lcurl -lssl -lcrypto #-lwsock32 -lws2_32 -lgdi32
