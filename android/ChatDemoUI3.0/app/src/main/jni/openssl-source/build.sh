./Configure --prefix=/d/workspace/tarballs/prebuilt/ssl-${ANDROID_EABI} \
	-fpic -fPIC \
	no-asm no-dso no-shared no-zlib \
	$1 # `android-arm' or `android-x86'

make depend

make all

make install
