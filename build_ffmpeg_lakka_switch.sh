#!/bin/sh

INSTALL_DIR=`pwd`/third_party/ffmpeg
git clone https://github.com/FFmpeg/FFmpeg.git ffmpeg_tmp
cd ffmpeg_tmp

# Fix TOOLCHAIN path!
TOOLCHAIN=/home/rock88/Documents/Lakka-LibreELEC/build.Lakka-L4T.aarch64-2.2-devel/toolchain

TARGET_CPU="cortex-a57"
TARGET_CPU_FLAGS="+crypto+crc+fp+simd"
TARGET_SUBARCH=aarch64
TARGET_ARCH=aarch64
TARGET_VARIANT=armv8-a
TARGET_ABI=eabi
SIMD_SUPPORT="yes"
TARGET_KERNEL_ARCH=arm64
TARGET_NAME=$TARGET_SUBARCH-libreelec-linux-gnu${TARGET_ABI}

SYSROOT_PREFIX=$TOOLCHAIN/$TARGET_NAME/sysroot
TARGET_PREFIX=$TOOLCHAIN/bin/$TARGET_NAME-

HOST_CC=$TOOLCHAIN/bin/host-gcc
HOST_CXX=$TOOLCHAIN/bin/host-g++

CC="${TARGET_PREFIX}gcc"
CXX="${TARGET_PREFIX}g++"
CPP="${TARGET_PREFIX}cpp"
LD="${TARGET_PREFIX}ld"
AS="${TARGET_PREFIX}as"
AR="${TARGET_PREFIX}ar"
NM="${TARGET_PREFIX}nm"
RANLIB="${TARGET_PREFIX}ranlib"

TARGET_CFLAGS="-mcpu=${TARGET_CPU}${TARGET_CPU_FLAGS} -mabi=lp64 -Wno-psabi -mtune=${TARGET_CPU} -march=${TARGET_VARIANT}${TARGET_CPU_FLAGS}"
TARGET_LDFLAGS="-mcpu=${TARGET_CPU}${TARGET_CPU_FLAGS}"
GCC_OPTS="--with-abi=lp64 --with-arch=$TARGET_VARIANT"

TARGET_CXXFLAGS="$TARGET_CFLAGS"
CPPFLAGS="$TARGET_CPPFLAGS"
CFLAGS="$TARGET_CFLAGS"
CXXFLAGS="$TARGET_CXXFLAGS"
LDFLAGS="$TARGET_LDFLAGS"

HOST_CPPFLAGS=""
HOST_CFLAGS="-O2 -Wall -pipe -I$TOOLCHAIN/include"
HOST_CXXFLAGS="$HOST_CFLAGS"
HOST_LDFLAGS="-Wl,-rpath,$TOOLCHAIN/lib -L$TOOLCHAIN/lib"
HOST_LIBDIR="$TOOLCHAIN/lib"

FFMPEG_FPU="--enable-neon"
FFMPEG_X11GRAB="--enable-indev=x11grab_xcb"
FFMPEG_TABLES="--disable-hardcoded-tables"
FFMPEG_DEBUG="--disable-debug --enable-stripping"
FFMPEG_RKMPP="--disable-rkmpp"
FFMPEG_VDPAU="--enable-vdpau"
FFMPEG_VAAPI="--disable-vaapi"

./configure \
--prefix=$INSTALL_DIR \
--cpu="$TARGET_CPU" \
--arch="$TARGET_ARCH" \
--enable-cross-compile \
--cross-prefix="$TARGET_PREFIX" \
--sysroot="$SYSROOT_PREFIX" \
--sysinclude="$SYSROOT_PREFIX/usr/include" \
--target-os="linux" \
--nm="$NM" \
--ar="$AR" \
--as="$CC" \
--cc="$CC" \
--ld="$CC" \
--host-cc="$HOST_CC" \
--host-cflags="$HOST_CFLAGS" \
--host-ldflags="$HOST_LDFLAGS" \
--extra-cflags="$CFLAGS" \
--extra-ldflags="$LDFLAGS" \
--extra-libs="$FFMPEG_LIBS" \
--enable-static \
--disable-shared \
--enable-gpl \
--disable-version3 \
--enable-nonfree \
--enable-logging \
--disable-doc \
$FFMPEG_DEBUG \
--enable-pic \
--pkg-config="$TOOLCHAIN/bin/pkg-config" \
--enable-optimizations \
--disable-extra-warnings \
--disable-ffprobe \
--disable-ffplay \
--enable-ffmpeg \
--enable-avdevice \
--enable-avcodec \
--enable-avformat \
--enable-swscale \
--enable-postproc \
--enable-avfilter \
--disable-devices \
--enable-pthreads \
--disable-w32threads \
--enable-network \
--disable-gnutls --enable-openssl \
--disable-gray \
--enable-swscale-alpha \
--disable-small \
--enable-dct \
--enable-fft \
--enable-mdct \
--enable-rdft \
--disable-crystalhd \
$FFMPEG_VAAPI \
$FFMPEG_VDPAU \
$FFMPEG_RKMPP \
--disable-dxva2 \
--enable-runtime-cpudetect \
$FFMPEG_TABLES \
--disable-encoders \
--enable-encoder=ac3 \
--enable-encoder=aac \
--enable-encoder=wmav2 \
--enable-encoder=mjpeg \
--enable-encoder=png \
--disable-decoder=mpeg_xvmc \
--enable-hwaccels \
--disable-muxers \
--enable-muxer=spdif \
--enable-muxer=adts \
--enable-muxer=asf \
--enable-muxer=ipod \
--enable-muxer=mpegts \
--enable-demuxers \
--enable-parsers \
--enable-bsfs \
--enable-protocol=http \
--disable-indevs \
--disable-outdevs \
--enable-filters \
--disable-avisynth \
--enable-bzlib \
--disable-frei0r \
--disable-libopencore-amrnb \
--disable-libopencore-amrwb \
--disable-libopencv \
--disable-libdc1394 \
--disable-libfreetype \
--disable-libgsm \
--disable-libmp3lame \
--disable-libopenjpeg \
--disable-librtmp \
--disable-libtheora \
--disable-libvo-amrwbenc \
--disable-libvorbis \
--disable-libvpx \
--disable-libx264 \
--disable-libxavs \
--disable-libxvid \
--enable-zlib \
--enable-asm \
--disable-altivec \
$FFMPEG_FPU \
--enable-yasm \
--disable-symver \
$FFMPEG_X11GRAB

make install
cd ..
rm -fr ffmpeg_tmp
