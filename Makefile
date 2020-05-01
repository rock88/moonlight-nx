ifeq ($(platform),)
	platform = unix
	ifeq ($(shell uname -a),)
		platform = win
	else ifneq ($(findstring MINGW,$(shell uname -a)),)
		platform = win
	else ifneq ($(findstring Darwin,$(shell uname -a)),)
		platform = osx
		arch = intel
	ifeq ($(shell uname -p),powerpc)
		arch = ppc
	endif
	else ifneq ($(findstring win,$(shell uname -a)),)
		platform = win
	endif
endif

# system platform
system_platform = unix
ifeq ($(shell uname -a),)
	EXE_EXT = .exe
	system_platform = win
else ifneq ($(findstring Darwin,$(shell uname -a)),)
	system_platform = osx
	arch = intel
	ifeq ($(shell uname -p),powerpc)
		arch = ppc
	endif
	else ifneq ($(findstring MINGW,$(shell uname -a)),)
	system_platform = win
endif

TARGET_NAME = moonlight

ifeq ($(platform), unix)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lGL
else ifneq (,$(findstring osx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro.dylib
   fpic := -fPIC
   SHARED := -dynamiclib
   GL_LIB := -framework OpenGL
   CFLAGS += -DOSX
	 INCLUDES += -I/usr/local/include -I/usr/local/opt/openssl@1.1/include
	 LIBS += -L/usr/local/lib -L/usr/local/opt/openssl@1.1/lib
	ifeq ($(arch),ppc)
		CFLAGS += -D__ppc__ -DOSX_PPC
	endif
	OSXVER = `sw_vers -productVersion | cut -d. -f 2`
	OSX_LT_MAVERICKS = `(( $(OSXVER) <= 9)) && echo "YES"`
	fpic += -mmacosx-version-min=10.12
else ifeq ($(platform), pi)
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CFLAGS += -I/opt/vc/include -I/opt/vc/include/interface/vcos/pthreads -I/opt/vc/include/vmcs_host/linux
   GLES := 1
   LIBS += -L/opt/vc/lib
else ifneq (,$(findstring ios,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_ios.dylib
   fpic := -fPIC
   SHARED := -dynamiclib

ifeq ($(IOSSDK),)
   IOSSDK := $(shell xcodebuild -version -sdk iphoneos Path)
endif
   GL_LIB := -framework OpenGLES
   DEFINES += -DIOS
   CFLAGS += -DHAVE_OPENGLES
   CC = cc -arch armv7 -isysroot $(IOSSDK)
ifeq ($(platform),ios9)
	CC     += -miphoneos-version-min=8.0
	CFLAGS += -miphoneos-version-min=8.0
else
	CC     += -miphoneos-version-min=5.0
	CFLAGS += -miphoneos-version-min=5.0
endif
else ifneq (,$(findstring qnx,$(platform)))
   TARGET := $(TARGET_NAME)_libretro_qnx.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T
   GL_LIB := -lGL

   CC = qcc -Vgcc_ntoarmv7le
   AR = qcc -Vgcc_ntoarmv7le
   GL_LIB := -lGLESv2
   GLES := 1
else ifneq (,$(findstring armv,$(platform)))
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.so
   fpic := -fPIC
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
   CFLAGS += -I.
ifneq (,$(findstring gles,$(platform)))
   GLES := 1
else
   GL_LIB := -lGL
endif
ifneq (,$(findstring cortexa8,$(platform)))
   CFLAGS += -marm -mcpu=cortex-a8
else ifneq (,$(findstring cortexa9,$(platform)))
   CFLAGS += -marm -mcpu=cortex-a9
endif
   CFLAGS += -marm
ifneq (,$(findstring neon,$(platform)))
   CFLAGS += -mfpu=neon
endif
ifneq (,$(findstring softfloat,$(platform)))
   CFLAGS += -mfloat-abi=softfp
else ifneq (,$(findstring hardfloat,$(platform)))
   CFLAGS += -mfloat-abi=hard
endif
   CFLAGS += -DARM
# emscripten
else ifeq ($(platform), emscripten)
	TARGET := $(TARGET_NAME)_libretro_emscripten.bc
# Lakka Switch
else ifeq ($(platform), lakka_switch)
   LAKKA_ROOT := /Users/rock88/Documents/Projects/RetroArch/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu
	 LAKKA_TOOLCHAIN := $(LAKKA_ROOT)/toolchain
	 CC = $(LAKKA_TOOLCHAIN)/bin/aarch64-libreelec-linux-gnueabi-gcc
	 TARGET := $(TARGET_NAME)_libretro.so
	 CFLAGS += -mcpu=cortex-a57+crypto+crc+fp+simd -mabi=lp64 -Wno-psabi -mtune=cortex-a57 \
	 -march=armv8-a+crypto+crc+fp+simd -fomit-frame-pointer -Wall -pipe -O2 \
	 -mcpu=cortex-a57 -std=c99 -fomit-frame-pointer -fPIC -pthread \
	 -I/home/rock88/Documents/Lakka-LibreELEC/build.Lakka-L4T.aarch64-2.2-devel/toolchain/aarch64-libreelec-linux-gnueabi/sysroot/usr/include \
	 -Wdeclaration-after-statement -Wall -Wdisabled-optimization -Wpointer-arith -Wredundant-decls -Wwrite-strings \
	 -Wtype-limits -Wundef -Wmissing-prototypes -Wno-pointer-to-int-cast -Wstrict-prototypes -Wempty-body \
	 -Wno-parentheses -Wno-switch -Wno-format-zero-length -Wno-pointer-sign -Wno-unused-const-variable -O3 \
	 -fno-math-errno -fno-signed-zeros -fno-tree-vectorize -Werror=format-security -Werror=implicit-function-declaration \
	 -Werror=missing-prototypes -Werror=return-type -Werror=vla -Wformat -fdiagnostics-color=auto -Wno-maybe-uninitialized
else
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32
   CFLAGS += -I..
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
else
   CFLAGS += -O3
endif

ifneq (,$(findstring qnx,$(platform)))
	CFLAGS += -Wc,-std=c99
else
	CFLAGS += -std=gnu99
endif

LIBGAMESTREAM_SOURCES = \
	libgamestream/client.c \
	libgamestream/http.c \
	libgamestream/mkcert.c \
	libgamestream/xml.c

MOONLIGHT_LIBRETRO_C_SOURCES = \
	src/glsym/rglgen.c \
	src/nanogui_resources/nanogui_resources.c \
	src/moonlight_libretro.c

MOONLIGHT_LIBRETRO_CXX_SOURCES = \
	src/ui/AddHostWindow.cpp \
	src/ui/Application.cpp \
	src/ui/ContentWindow.cpp \
	src/ui/LoadingOverlay.cpp \
	src/ui/MainWindow.cpp \
	src/Server.cpp \
	src/moonlight_libretro_wrapper.cpp

MOONLIGHT_COMMON_C_SOURCES = \
	third_party/moonlight-common-c/enet/callbacks.c \
	third_party/moonlight-common-c/enet/compress.c \
	third_party/moonlight-common-c/enet/host.c \
	third_party/moonlight-common-c/enet/list.c \
	third_party/moonlight-common-c/enet/packet.c \
	third_party/moonlight-common-c/enet/peer.c \
	third_party/moonlight-common-c/enet/protocol.c \
	third_party/moonlight-common-c/enet/unix.c \
	third_party/moonlight-common-c/enet/win32.c \
	third_party/moonlight-common-c/reedsolomon/rs.c \
	third_party/moonlight-common-c/src/AudioStream.c \
	third_party/moonlight-common-c/src/ByteBuffer.c \
	third_party/moonlight-common-c/src/Connection.c \
	third_party/moonlight-common-c/src/ControlStream.c \
	third_party/moonlight-common-c/src/FakeCallbacks.c \
	third_party/moonlight-common-c/src/InputStream.c \
	third_party/moonlight-common-c/src/LinkedBlockingQueue.c \
	third_party/moonlight-common-c/src/Misc.c \
	third_party/moonlight-common-c/src/Platform.c \
	third_party/moonlight-common-c/src/PlatformSockets.c \
	third_party/moonlight-common-c/src/RtpFecQueue.c \
	third_party/moonlight-common-c/src/RtpReorderQueue.c \
	third_party/moonlight-common-c/src/RtspConnection.c \
	third_party/moonlight-common-c/src/RtspParser.c \
	third_party/moonlight-common-c/src/SdpGenerator.c \
	third_party/moonlight-common-c/src/SimpleStun.c \
	third_party/moonlight-common-c/src/VideoDepacketizer.c \
	third_party/moonlight-common-c/src/VideoStream.c

NANOGUI_C_SOURCES = \
	third_party/nanogui/ext/nanovg/src/nanovg.c

NANOGUI_CXX_SOURCES = \
	third_party/nanogui/src/widget.cpp \
	third_party/nanogui/src/button.cpp \
	third_party/nanogui/src/common.cpp \
	third_party/nanogui/src/screen.cpp \
	third_party/nanogui/src/checkbox.cpp \
	third_party/nanogui/src/vscrollpanel.cpp \
	third_party/nanogui/src/colorpicker.cpp \
	third_party/nanogui/src/textarea.cpp \
	third_party/nanogui/src/shader_gl.cpp \
	third_party/nanogui/src/canvas.cpp \
	third_party/nanogui/src/window.cpp \
	third_party/nanogui/src/graph.cpp \
	third_party/nanogui/src/popup.cpp \
	third_party/nanogui/src/layout.cpp \
	third_party/nanogui/src/texture.cpp \
	third_party/nanogui/src/texture_gl.cpp \
	third_party/nanogui/src/tabwidget.cpp \
	third_party/nanogui/src/shader.cpp \
	third_party/nanogui/src/imageview.cpp \
	third_party/nanogui/src/progressbar.cpp \
	third_party/nanogui/src/combobox.cpp \
	third_party/nanogui/src/theme.cpp \
	third_party/nanogui/src/traits.cpp \
	third_party/nanogui/src/label.cpp \
	third_party/nanogui/src/opengl.cpp \
	third_party/nanogui/src/renderpass_gl.cpp \
	third_party/nanogui/src/imagepanel.cpp \
	third_party/nanogui/src/colorwheel.cpp \
	third_party/nanogui/src/messagedialog.cpp \
	third_party/nanogui/src/textbox.cpp \
	third_party/nanogui/src/slider.cpp \
	third_party/nanogui/src/popupbutton.cpp

INCLUDES += \
	-Isrc \
	-Isrc/nanogui_resources \
	-Isrc/ui \
	-Ilibgamestream \
	-Ithird_party/moonlight-common-c/reedsolomon \
	-Ithird_party/moonlight-common-c/src \
	-Ithird_party/moonlight-common-c/enet/include \
	-Ithird_party/nanogui/include \
	-Ithird_party/nanogui/ext/nanovg/src

C_SOURCES = $(LIBGAMESTREAM_SOURCES) $(MOONLIGHT_LIBRETRO_C_SOURCES) $(MOONLIGHT_COMMON_C_SOURCES) $(NANOGUI_C_SOURCES)
CXX_SOURCES = $(MOONLIGHT_LIBRETRO_CXX_SOURCES) $(NANOGUI_CXX_SOURCES)

OBJECTS := $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o)
DEFINES += -DNANOGUI_USE_OPENGL -DNVG_STB_IMAGE_IMPLEMENTATION -DNANOGUI_NO_GLFW
CFLAGS += -Wall -pedantic $(fpic) -std=gnu11 $(DEFINES)
CXXFLAGS += -std=gnu++17 -stdlib=libc++ $(DEFINES)

LIBS += \
	-lcrypto -lssl -lcurl -lz -lexpat \
	-lavcodec -lavformat -lavutil -lavdevice -lstdc++

ifeq ($(GLES), 1)
   CFLAGS += -DHAVE_OPENGLES -DHAVE_OPENGLES2
   ifeq ($(GLES31), 1)
      CFLAGS += -DHAVE_OPENGLES3 -DHAVE_OPENGLES_3_1
   else ifeq ($(GLES3), 1)
      CFLAGS += -DHAVE_OPENGLES3
   endif
   LIBS += -lGLESv2 # Still link against GLESv2 when using GLES3 API, at least on desktop Linux.
   OBJECTS += src/glsym/glsym_es2.o
else
   OBJECTS += src/glsym/glsym_gl.o
   LIBS += $(GL_LIB)
endif

ifeq ($(CORE), 1)
   CFLAGS += -DCORE
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(fpic) $(SHARED) $(INCLUDES) -o $@ $(OBJECTS) $(LIBS) -lm $(EXTRA_GL_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(fpic) $(INCLUDES) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(fpic) $(INCLUDES) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	/Applications/RetroArch.app/Contents/MacOS/RetroArch -L $(TARGET)

.PHONY: clean
