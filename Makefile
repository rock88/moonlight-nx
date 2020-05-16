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
   LIBS += -lpthread -lGL -luuid -Lthird_party/opus/lib
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
# Lakka Switch
else ifeq ($(platform), lakka-switch)
   TOOLCHAIN := /home/rock88/Documents/Lakka-LibreELEC/build.Lakka-L4T.aarch64-2.2-devel/toolchain
   CC = $(TOOLCHAIN)/bin/aarch64-libreelec-linux-gnueabi-gcc
   CXX = $(TOOLCHAIN)/bin/aarch64-libreelec-linux-gnueabi-c++
   TARGET := $(TARGET_NAME)_libretro.so
   DEFINES += -mcpu=cortex-a57+crypto+crc+fp+simd -mabi=lp64 -Wno-psabi -mtune=cortex-a57 \
	 -march=armv8-a+crypto+crc+fp+simd -fomit-frame-pointer -Wall -pipe -fPIC -pthread \
	 -D__LAKKA_SWITCH__
   INCLUDES += -Ithird_party/opus/include -Ithird_party/ffmpeg/include \
	 -I$(TOOLCHAIN)/aarch64-libreelec-linux-gnueabi/sysroot/usr/include -I$(TOOLCHAIN)/include
   LIBS += -Lthird_party/opus/lib -Lthird_party/ffmpeg/lib \
	 -lpthread -lGL -luuid -lvdpau -lX11
   SHARED := -shared -Wl,--version-script=link.T -Wl,--no-undefined
else
   CC = gcc
   TARGET := $(TARGET_NAME)_libretro.dll
   SHARED := -shared -static-libgcc -static-libstdc++ -s -Wl,--version-script=link.T -Wl,--no-undefined
   GL_LIB := -lopengl32
   CFLAGS += -I..
endif

ifeq ($(DEBUG), 1)
   CFLAGS += -O0 -g
   CXXFLAGS += -O0 -g
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
	src/ui/windows/AddHostWindow.cpp \
	src/ui/windows/ContentWindow.cpp \
	src/ui/windows/MainWindow.cpp \
	src/ui/windows/StreamWindow.cpp \
	src/ui/windows/AppListWindow.cpp \
	src/ui/windows/SettingsWindow.cpp \
	src/ui/buttons/AddHostButton.cpp \
	src/ui/buttons/AppButton.cpp \
	src/ui/buttons/HostButton.cpp \
	src/ui/Application.cpp \
	src/ui/LoadingOverlay.cpp \
	src/GameStreamClient.cpp \
	src/Settings.cpp \
	src/InputController.cpp \
	src/streaming/MoonlightSession.cpp \
	src/streaming/audio/RetroAudioRenderer.cpp \
	src/streaming/ffmpeg/FFmpegVideoDecoder.cpp \
	src/streaming/video/GLVideoRenderer.cpp

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
	-Isrc -Isrc/decoders -Isrc/glsym \
	-Isrc/nanogui_resources \
	-Isrc/ui -Isrc/ui/buttons -Isrc/ui/windows \
	-Isrc/streaming -Isrc/streaming/audio -Isrc/streaming/video -Isrc/streaming/ffmpeg \
	-Ilibgamestream \
	-Ithird_party/moonlight-common-c/reedsolomon \
	-Ithird_party/moonlight-common-c/src \
	-Ithird_party/moonlight-common-c/enet/include \
	-Ithird_party/nanogui/include \
	-Ithird_party/nanogui/ext/nanovg/src \
	-Ithird_party/json/single_include/nlohmann

C_SOURCES = $(LIBGAMESTREAM_SOURCES) $(MOONLIGHT_LIBRETRO_C_SOURCES) $(MOONLIGHT_COMMON_C_SOURCES) $(NANOGUI_C_SOURCES)
CXX_SOURCES = $(MOONLIGHT_LIBRETRO_CXX_SOURCES) $(NANOGUI_CXX_SOURCES)

OBJECTS := $(C_SOURCES:.c=.o) $(CXX_SOURCES:.cpp=.o)
DEFINES += -DNANOGUI_USE_OPENGL -DNVG_STB_IMAGE_IMPLEMENTATION -DNANOGUI_NO_GLFW \
	-DHAS_SOCKLEN_T -DHAS_POLL -DHAS_FCNTL -D_GNU_SOURCE -D__LIBRETRO__

CFLAGS += -Wall -pedantic $(fpic) -std=gnu11 $(DEFINES)
CXXFLAGS += -std=gnu++17 $(DEFINES)

LIBS += -lcrypto -lssl -lcurl -lz -lexpat -lopus \
	-lavcodec -lavformat -lavutil -lavdevice -lstdc++ -lswresample

OBJECTS += src/glsym/glsym_gl.o
LIBS += $(GL_LIB)

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
