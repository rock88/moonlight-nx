![Build](https://github.com/rock88/moonlight-nx/workflows/Build/badge.svg)

# Moonlight-NX

Moonlight-NX is a port of [Moonlight Game Streaming Project](https://github.com/moonlight-stream "Moonlight Game Streaming Project") for Nintendo Switch.

# Installing
1. Download latest Moonlight-NX [release](https://github.com/rock88/moonlight-nx/releases) or [automatic build](https://github.com/rock88/moonlight-nx/actions?query=workflow%3ABuild+is%3Asuccess) (require github login for artifacts link appear);
2. Put moonlight.nro to sdcard:/switch/moonlight;
3. Launch hbmenu over *Title Redirection* (for FULL RAM access);
4. Launch moonlight.

# Controls
## Mouse
With touch screen you can configure two mouse input mode: 
- Moving the cursor along the movement your finger (by default);
- Immediately move cursor by tap area and perform left mouse click (enable `Mouse click by tap on screen` option in Settings).

In both case you can hold L/R buttons an tap on a screen for left/right click (cursor will not move) or hold ZL/ZR for allow cursor movement while left/right mouse buttons holded.

Also you can scroll pages with two fingers.

## Keyboard
Currently full keyboard input not supported, but you can use this combinations for most common cases:

1. Minus+Plus - Guide button;
2. ZL+ZR+Up - Exit from stream (without closing current app);
3. ZL+ZR+Down - Exit from stream (and close current app);
4. ZL+ZR+Left - Show/hide video decoder/render stats;
5. L+R+Up - ESC key;
6. L+R+Down - Win key;
7. L+R+Left - Alt+Enter (for enable/disable fullscreen mode in some games or emulators);
8. L+R+Right - Shift+Tab (for open/close Steam overlay).

You can change this combos on a Input Settings screen (Gamepad icon on a main screen, next tap keyboard icon) or by press Y button on a Applications screen for adjust keyboard combos for a specific game/application.

## Gamepad
By default Switch gamepad configured as X360 gamepad (A/B and X/Y swapped). You can reconfigure buttons on a Input Settings screen or by press Y button on a Applications screen for adjust gamepad mapping for a specific game/application.

Multiple gamepad currently not supported.

## HID Keyboard/Mouse
Moonlight-NX support attached hardware keyboard/mouse over USB connected on Switch directly or over dock (Switch should be in dock).

# Build Moonlight-NX
## Using Docker
1. Create a working dir: 

`mkdir moonlight-nx-build; cd moonlight-nx-build`

2. Download a Dockerfile:

`wget https://raw.githubusercontent.com/rock88/moonlight-nx/master/Dockerfile`

3. Build and run a development environment: 

```
docker build . -t moonlight-nx-build
docker run -it moonlight-nx-build
```

4. Clone the repo: 

`git clone --recursive https://github.com/rock88/moonlight-nx.git`

5. Run the build:

`cd moonlight-nx; make; exit`

6. Copy the .nro out of the container: 
```
// Find your container ID
docker ps -a

// Use container ID for copy .nro from container
docker cp CONTAINER_ID:moonlight-nx/moonlight.nro .
```

## Build Manually
1. Setup a [Development Environment](https://switchbrew.org/wiki/Setting_up_Development_Environment "Development Environment")
2. Install deps (`dkp-pacman` for Linux like arch, `pacman` for MacOS) and update all to latest versions:

```
sudo (dkp-)pacman --noconfirm -S devkitA64 switch-tools libnx switch-ffmpeg switch-glad switch-glfw switch-jansson switch-libdrm_nouveau switch-libexpat switch-libopus switch-libvpx switch-mbedtls switch-mesa switch-zlib switch-ffmpeg switch-curl
sudo (dkp-)pacman --noconfirm -Suy
```

3. Clone the repo: 

`git clone --recursive https://github.com/rock88/moonlight-nx.git`

4. Run the build:

`cd moonlight-nx; make -j`

# Assets
Icon - [moonlight-stream](https://github.com/moonlight-stream "moonlight-stream") project logo.
