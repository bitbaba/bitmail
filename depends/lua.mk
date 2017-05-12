# for mingw build on windows, use:
# make -f lua.mk PLATFORM=generic INSTALLROOT=/d/workspace/github/bitmail/out

package=lua
package_version=5.3.4
package_download_url=http://www.lua.org/ftp/lua-5.3.4.tar.gz
package_download_sig=
package_extract_dir=/tmp
package_download_dir=/tmp
package_tarball=$(package_download_dir)/$(package)-$(package_version).tar.gz
package_build_dir=$(package_extract_dir)/$(package)-$(package_version)
package_output_dir=$${INSTALLROOT:=/d/workspace/github/bitmail/out}
package_platform=$${PLATFORM:=generic}


all: $(package_tarball)
	tar zxvf $(package_tarball) -C $(package_extract_dir) && \
	cd $(package_build_dir) && \
	make $(package_platform) && \
	make install INSTALL_TOP=$(package_output_dir) && \
	make clean

$(package_tarball):
	curl -k $(package_download_url) -o $(package_tarball)
