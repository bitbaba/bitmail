package=curl
package_version=7.48.0
package_download_url=https://curl.haxx.se/download/curl-7.48.0.tar.gz
package_download_sig=
package_extract_dir=/tmp
package_download_dir=/tmp
package_tarball=$(package_download_dir)/$(package)-$(package_version).tar.gz
package_build_dir=$(package_extract_dir)/$(package)-$(package_version)
package_output_dir=$${INSTALLROOT}


all: download
	tar zxvf $(package_tarball) -C $(package_extract_dir) && \
	cd $(package_build_dir) && \
	./configure --prefix=$(package_output_dir) \
				--with-ssl=$(package_output_dir) \
				--disable-shared \
				--enable-static \
				--disable-ldap \
				--disable-ldaps \
				--without-zlib && \
	make && \
	make install && \
	make distclean

download:
	if ! [ -f $(package_tarball) ]; then curl -k $(package_download_url) -o $(package_tarball); fi;

.PHONY : all download