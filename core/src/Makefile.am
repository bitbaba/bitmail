AM_CPPFLAGS=-I$(top_srcdir)/include

lib_LTLIBRARIES=libbitmailcore.la

libbitmailcore_la_SOURCES= \
    bitmail.cpp \
    x509cert.cpp \
    email.cpp 

libbitmailcore_la_CFLAGS  = -DCURL_STATICLIB
libbitmailcore_la_LDFLAGS = -no-undefined -export-all-symbols -compat-implib -out-implib libbitmailcore.dll.a
libbitmailcore_la_LIBADD  = -lcurl -lssl -lcrypto
