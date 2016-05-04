all:
	g++ \
        -DTEST \
	-I/root/workspace/github/bitmail/out/include \
	-o bmc \
	-g -O0 \
	../src/bitmail.cpp \
	../src/email.cpp \
	../src/x509cert.cpp \
	-L/root/workspace/github/bitmail/out/lib \
        -lcurl -lssl -lcrypto -lpthread
