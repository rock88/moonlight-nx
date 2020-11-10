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
1. Build a development environment: `docker build . -t moonlight-nx-build`
2. Run run the development environment: `docker run -it moonlight-nx-build`
3. Clone this repo: `git clone --recursive https://github.com/rock88/moonlight-nx.git`;
4. Set environment variables for the custom libcurl build: `export LIBCURL_PKGBUILD_URL="https://github.com/devkitPro/pacman-packages/raw/1582ad85914b14497fae32a9fe9074c0374f99f7/switch/curl/PKGBUILD"; export LIBCURL_BUILD_USER="build"`
5. Run the build: `cd moonlight-nx; ./build.sh; exit`.
6. Copy the .nro out of the container: `docker cp moonlight-nx-build:moonlight-nx/moonlight.nro .`

# Assets
Icon - [moonlight-stream](https://github.com/moonlight-stream "moonlight-stream") project logo.
