#!/bin/sh

INSTALL_DIR=`pwd`/third_party/opus
git clone https://github.com/xiph/opus.git opus_tmp
cd opus_tmp
./autogen.sh

# Fix LAKKA_TOOLCHAIN path!
LAKKA_TOOLCHAIN=/home/rock88/Documents/Lakka-LibreELEC/build.Lakka-L4T.aarch64-2.2-devel/toolchain
CFLAGS='-mcpu=cortex-a57+crypto+crc+fp+simd -mabi=lp64 -Wno-psabi -mtune=cortex-a57 -march=armv8-a+crypto+crc+fp+simd -fomit-frame-pointer -Wall -pipe -O2 -mcpu=cortex-a57 -fPIC' \
ASFLAGS='-mcpu=cortex-a57+crypto+crc+fp+simd -mabi=lp64 -Wno-psabi -mtune=cortex-a57 -march=armv8-a+crypto+crc+fp+simd -fomit-frame-pointer -Wall -pipe -O2 -mcpu=cortex-a57 -fPIC' \
CC=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-gcc \
LD=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-ld \
AR=$LAKKA_TOOLCHAIN/bin/aarch64-libreelec-linux-gnueabi-ar \
./configure --host=aarch64-libreelec-linux-gnueabi --prefix=$INSTALL_DIR
make install
cd ..
rm -fr opus_tmp
