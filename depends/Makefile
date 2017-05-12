#depends_out_dir=$${INSTALLROOT:=/usr/local}
depends_out_dir=$${INSTALLROOT:=/d/workspace/github/bitmail/out}
depends_lua_platform=$${LUA_PLATFORM:=generic}
depends_openssl_cflags=$${OPENSSL_CFLAGS}
depends_openssl_platform=$${OPENSSL_PLATFORM:=mingw}

all: depends

depends:
	#INSTALLROOT=$(depends_out_dir); echo $${INSTALLROOT}; sleep 10;
	INSTALLROOT=$(depends_out_dir) OPENSSL_CFLAGS="$(depends_openssl_cflags)" OPENSSL_PLATFORM=$(depends_openssl_platform) $(MAKE) -f openssl.mk 
	INSTALLROOT=$(depends_out_dir) $(MAKE) -f curl.mk 
	INSTALLROOT=$(depends_out_dir) LUA_PLATFORM=$(depends_lua_platform) $(MAKE) -f lua.mk 
	INSTALLROOT=$(depends_out_dir) $(MAKE) -f microhttpd.mk 
	INSTALLROOT=$(depends_out_dir) $(MAKE) -f qrencode.mk 

.PHONY: depends