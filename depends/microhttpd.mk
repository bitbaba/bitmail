package=libmicrohttpd
package_version=0.9.51
package_download_url="http://ftp.gnu.org/gnu/libmicrohttpd/libmicrohttpd-0.9.51.tar.gz"
package_download_sig=
package_extract_dir=/tmp
package_download_dir=/tmp
package_tarball=$(package_download_dir)/$(package)-$(package_version).tar.gz
package_build_dir=$(package_extract_dir)/$(package)-$(package_version)
package_output_dir=$${INSTALLROOT}


all: download
	tar zxvf $(package_tarball) -C $(package_extract_dir) && \
	cd $(package_build_dir) && \
    ./configure   --prefix=$(package_output_dir) \
                    --enable-shared=no \
                    --enable-static=yes \
                    --enable-https=no \
                    --enable-bauth \
                    --enable-dauth \
                    --disable-curl \
                    --disable-doc \
                    --disable-examples \
                    --disable-poll \
                    --disable-epoll && \
	make && \
	make install && \
	make clean

download:
	if ! [ -f $(package_tarball) ]; then curl -k $(package_download_url) -o $(package_tarball); fi;

.PHONY : all download