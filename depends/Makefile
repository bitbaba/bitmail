

all: depends

depends:
	$(MAKE) -f openssl.mk
	$(MAKE) -f curl.mk
	$(MAKE) -f lua.mk
	$(MAKE) -f microhttpd.mk
	$(MAKE) -f qrencode.mk

.PHONY: depends