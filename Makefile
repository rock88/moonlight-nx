#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
TARGET		:=	moonlight
APP_TITLE	:= Moonlight
ICON		:= icon.jpg
APP_AUTHOR	:= port by rock88
APP_VERSION	:= 1.2.0
BUILD		:=	build
SOURCES		:=	src src/libgamestream src/switch src/nanogui_resources src/streaming src/streaming/ffmpeg \
	src/crypto src/streaming/video src/crypto src/streaming/audio src/ui/windows src/ui/buttons src/ui src/utils src/controls\
	third_party/moonlight-common-c/enet third_party/moonlight-common-c/reedsolomon third_party/moonlight-common-c/src \
	third_party/nanogui/ext/nanovg/src third_party/nanogui/src
DATA		:=	data
INCLUDES	:=	include
#ROMFS	:=	romfs

ifdef NIGHTLY_BUILD
	MOONLIGHT_VERSION := "$(APP_VERSION)($(NIGHTLY_BUILD))"
else
	MOONLIGHT_VERSION := "$(APP_VERSION)"
endif

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

M_INCLUDES := \
	-I$(TOPDIR)/src -I$(TOPDIR)/src/switch -I$(TOPDIR)/src/streaming -I$(TOPDIR)/src/crypto -I$(TOPDIR)/src/crypto/keys \
	-I$(TOPDIR)/src/streaming/ffmpeg -I$(TOPDIR)/src/streaming/video -I$(TOPDIR)/src/streaming/audio \
	-I$(TOPDIR)/src/nanogui_resources -I$(TOPDIR)/src/utils -I$(TOPDIR)/src/controls \
	-I$(TOPDIR)/src/ui -I$(TOPDIR)/src/ui/buttons -I$(TOPDIR)/src/ui/windows \
	-I$(TOPDIR)/src/libgamestream \
	-I$(TOPDIR)/third_party/moonlight-common-c/reedsolomon \
	-I$(TOPDIR)/third_party/moonlight-common-c/src \
	-I$(TOPDIR)/third_party/moonlight-common-c/enet/include \
	-I$(TOPDIR)/third_party/nanogui/include \
	-I$(TOPDIR)/third_party/nanogui/ext/nanovg/src

DEFINES := -DNANOGUI_USE_OPENGL -DNVG_STB_IMAGE_IMPLEMENTATION -DNANOGUI_NO_GLFW \
	-DHAS_SOCKLEN_T -DHAS_POLL -DHAS_FCNTL -DUSE_MBEDTLS_CRYPTO -DMOONLIGHT_VERSION=\"$(MOONLIGHT_VERSION)\"

CFLAGS	:=	-g -Wall -fcompare-debug-second -O2 -ffunction-sections $(ARCH) $(DEFINES) $(INCLUDE) $(M_INCLUDES) -D__SWITCH__
CXXFLAGS	:= $(CFLAGS) -std=gnu++17

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS	:= -lcurl -lmbedtls -lmbedx509 -lmbedcrypto \
	-lavcodec -lavutil -lopus -lz -lexpat \
	-lglad -lEGL -lglapi -ldrm_nouveau -lglfw3 \
	-lnx -lswresample -lvpx -ljansson

LIBGAMESTREAM_CPP_SOURCES = \
	client.cpp \
	http.cpp \
	xml.cpp

MOONLIGHT_LIBRETRO_C_SOURCES = \
	nanogui_resources.c \
	switch_wrapper.c

MOONLIGHT_LIBRETRO_CXX_SOURCES = \
	main.cpp \
	AddHostWindow.cpp \
	ContentWindow.cpp \
	MainWindow.cpp \
	StreamWindow.cpp \
	AppListWindow.cpp \
	SettingsWindow.cpp \
	AddHostButton.cpp \
	AppButton.cpp \
	HostButton.cpp \
	Application.cpp \
	LoadingOverlay.cpp \
	GameStreamClient.cpp \
	Settings.cpp \
	MoonlightSession.cpp \
	FFmpegVideoDecoder.cpp \
	GLVideoRenderer.cpp \
	Data.cpp \
	MbedTLSCryptoManager.cpp \
	mbedtls_to_openssl_wrapper.cpp \
	AudrenAudioRenderer.cpp \
	BoxArtManager.cpp \
	Logger.cpp \
	LogsWindow.cpp \
	GamepadMapper.cpp \
	InputSettingsWindow.cpp \
	Alert.cpp \
	WakeOnLanManager.cpp \
	MouseController.cpp \
	KeyboardController.cpp \
	GamepadController.cpp \
	StreamControlsController.cpp

MOONLIGHT_COMMON_C_SOURCES = \
	callbacks.c \
	compress.c \
	host.c \
	list.c \
	packet.c \
	peer.c \
	protocol.c \
	unix.c \
	win32.c \
	rs.c \
	AudioStream.c \
	ByteBuffer.c \
	Connection.c \
	ControlStream.c \
	FakeCallbacks.c \
	InputStream.c \
	LinkedBlockingQueue.c \
	Misc.c \
	Platform.c \
	PlatformSockets.c \
	RtpFecQueue.c \
	RtpReorderQueue.c \
	RtspConnection.c \
	RtspParser.c \
	SdpGenerator.c \
	SimpleStun.c \
	VideoDepacketizer.c \
	VideoStream.c

NANOGUI_C_SOURCES = \
	nanovg.c

NANOGUI_CXX_SOURCES = \
	widget.cpp \
	button.cpp \
	common.cpp \
	screen.cpp \
	checkbox.cpp \
	vscrollpanel.cpp \
	colorpicker.cpp \
	textarea.cpp \
	shader_gl.cpp \
	canvas.cpp \
	window.cpp \
	graph.cpp \
	popup.cpp \
	layout.cpp \
	texture.cpp \
	texture_gl.cpp \
	tabwidget.cpp \
	shader.cpp \
	imageview.cpp \
	progressbar.cpp \
	combobox.cpp \
	theme.cpp \
	traits.cpp \
	label.cpp \
	opengl.cpp \
	renderpass_gl.cpp \
	imagepanel.cpp \
	colorwheel.cpp \
	messagedialog.cpp \
	textbox.cpp \
	slider.cpp \
	popupbutton.cpp
	
#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(TOPDIR)/curl $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
	$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(MOONLIGHT_LIBRETRO_C_SOURCES) $(MOONLIGHT_COMMON_C_SOURCES) $(NANOGUI_C_SOURCES)
CPPFILES	:=	$(LIBGAMESTREAM_CPP_SOURCES) $(MOONLIGHT_LIBRETRO_CXX_SOURCES) $(NANOGUI_CXX_SOURCES)
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------
all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
ifeq ($(strip $(APP_JSON)),)
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf
else
	@rm -fr $(BUILD) $(TARGET).nsp $(TARGET).nso $(TARGET).npdm $(TARGET).elf
endif


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)

all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
