![Build](https://github.com/rock88/moonlight-nx/workflows/Build/badge.svg)

# Moonlight-NX

Moonlight-NX is a port of [Moonlight Game Streaming Project](https://github.com/moonlight-stream "Moonlight Game Streaming Project") for Nintendo Switch.

# Installing
1. Download latest Moonlight-NX [release](https://github.com/rock88/moonlight-nx/releases) or [automatic build](https://github.com/rock88/moonlight-nx/actions?query=workflow%3ABuild+is%3Asuccess) (require github login for artifacts link appear);
2. Put moonlight.nro to sdcard:/switch/moonlight;
3. Launch hbmenu over *Title Redirection* (for FULL RAM access);
4. Launch moonlight.

# Controls (Defaults)
1. Move cursor with move finger on touch screen;
2. Scroll with two fingers;
3. L/R + tap on screen - Left/Right mouse click (allow to move cursor);
4. ZL/ZR + tap on screen - Left/Right mouse click (without move cursor);
5. L+R+Down - exit from stream (and close current app);
6. L+R+Up - exit from stream (without closing current app);
7. L+R+Left - Alt+Enter (for enable/disable fullscreen mode);
8. L+R+Right - ESC key;
9. ZL+ZR+Left - show video decoder/render stats;
10. ZL+ZR+Right - hide video decoder/render stats;
11. Minus+Plus - Guide button;

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
