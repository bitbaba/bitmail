package=openssl
package_version=1.0.2h
package_download_url="https://www.openssl.org/source/openssl-1.0.2h.tar.gz"
package_download_sig=
package_extract_dir=/tmp
package_download_dir=/tmp
package_tarball=$(package_download_dir)/$(package)-$(package_version).tar.gz
package_build_dir=$(package_extract_dir)/$(package)-$(package_version)
package_output_dir=$${INSTALLROOT:=/d/workspace/github/bitmail/out}
package_cflags=$${CFLAGS:=-fPIC -fpic}
package_platform=$${PLATFORM:=mingw} #darwin64-x86_64-cc|linux-x86_64

all: $(package_tarball)
	tar zxvf $(package_tarball) -C $(package_extract_dir) && \
	cd $(package_build_dir) && \
	./Configure --prefix=$(package_output_dir) $(package_cflags) \
			no-asm \
			no-zlib \
			no-shared \
			no-dso \
			$(package_platform) && \
	make && \
	make install && \
	make clean

$(package_tarball):
	curl -k $(package_download_url) -o $(package_tarball)
