#!/bin/sh

INSTALL_DIR=`pwd`/third_party/opus
git clone https://github.com/xiph/opus.git opus_tmp
cd opus_tmp
./autogen.sh

# Fix LAKKA_TOOLCHAIN path!
LAKKA_TOOLCHAIN=/home/rock88/Documents/Lakka-LibreELEC/build.Lakka-L4T.aarch64-2.2-devel/toolchain
CC=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-gcc \
LD=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-ld \
AR=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-ar \
./configure --host=aarch64-libreelec-linux-gnueabi --prefix=$INSTALL_DIR
make install
cd ..
rm -fr opus_tmp
