all:
	g++ \
        -DTEST \
	-I../../out/include \
	-o bmc \
	-g -O0 \
	../src/bitmail.cpp \
	../src/email.cpp \
	../src/x509cert.cpp \
	-L../../out/lib \
        -lcurl -lssl -lcrypto -lpthread
