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
1. Setup [Development Environment](https://switchbrew.org/wiki/Setting_up_Development_Environment "Development Environment");
2. Install deps (`dkp-pacman` for Ubuntu, `pacman` for MacOS):
    ```
    sudo (dkp-)pacman --noconfirm -S devkitA64 switch-tools libnx switch-ffmpeg switch-glad switch-glfw switch-jansson switch-libdrm_nouveau switch-libexpat switch-libopus switch-libvpx switch-mbedtls switch-mesa switch-zlib switch-ffmpeg switch-pkg-config devkitpro-pkgbuild-helpers
    ```
3. Install libcurl 7.69.1-1 (Please note - latest libcurl 7.69.1-2 currently unsupported! `dkp-makepkg` for Ubuntu, `makepkg` for MacOS. This will overwrite your current libcurl, be careful!):
    ```
    wget https://github.com/devkitPro/pacman-packages/raw/1582ad85914b14497fae32a9fe9074c0374f99f7/switch/curl/PKGBUILD
    (dkp-)makepkg
    sudo cp -r pkg/switch-curl/opt/devkitpro/portlibs/switch/* /opt/devkitpro/portlibs/switch
    ```
4. Clone this repo: `git clone --recursive https://github.com/rock88/moonlight-nx.git`;
5. `cd moonlight-nx`;
6. `make`.

# Assets
Icon - [moonlight-stream](https://github.com/moonlight-stream "moonlight-stream") project logo.
