package=qrencode
package_version=3.4.4
package_download_url="https://fukuchi.org/works/qrencode/qrencode-3.4.4.tar.gz"
package_download_sig=
package_extract_dir=/tmp
package_download_dir=/tmp
package_tarball=$(package_download_dir)/$(package)-$(package_version).tar.gz
package_build_dir=$(package_extract_dir)/$(package)-$(package_version)
package_output_dir=$${INSTALLROOT}


all: $(package_tarball)
	tar zxvf $(package_tarball) -C $(package_extract_dir) && \
	cd $(package_build_dir) && \
    ./configure --prefix=$(package_output_dir) \
				--enable-shared=no \
				--enable-static=yes \
				--without-tools && \
	make && \
	make install && \
	make clean

$(package_tarball):
	curl -k $(package_download_url) -o $(package_tarball)
