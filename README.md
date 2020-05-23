# Moonlight-NX

Moonlight-NX is a port of [Moonlight Game Streaming Project](https://github.com/moonlight-stream "Moonlight Game Streaming Project") for Nintendo Switch.

# Installing
1. Download latest Moonlight-NX [release](https://github.com/rock88/moonlight-nx/releases "release");
2. Put moonlight.nro to sdcard:/switch/moonlight;
3. Launch hbmenu with [FULL RAM access](https://gbatemp.net/threads/use-atmosphere-to-access-full-ram-with-homebrews-without-nsp.521240/ "FULL RAM access");
4. Launch moonlight.

# Controls
1. Move cursor with move finger on touch screen;
2. L/R + tap on screen - Left/Right mouse click (allow to move cursor);
3. ZL/ZR + tap on screen - Left/Right mouse click (without move cursor);
4. L+R+Down - exit from stream (and close current app);
5. L+R+Up - exit from stream (without closing current app);
6. L+R+Left - Alt+Enter (for enable/disable fullscreen mode);
7. L+R+Right - ESC key;
8. ZL+ZR+Left - show video decoder/render stats;
9. ZL+ZR+Right - hide video decoder/render stats;

# Build Moonlight-NX
1. Setup [Development Environment](https://switchbrew.org/wiki/Setting_up_Development_Environment "Development Environment");
2. Install deps (See [Makefile](https://github.com/rock88/moonlight-nx/blob/master/Makefile#L80 "Makefile")). Please note - you should use [libcurl 7.69.1](https://github.com/devkitPro/pacman-packages/commit/1582ad85914b14497fae32a9fe9074c0374f99f7 "libcurl 7.69.1")!
3. Clone this repo: `git clone --recursive https://github.com/rock88/moonlight-nx.git`;
4. `cd moonlight-nx`;
5. `make`.

# Assets
Icon - [moonlight-stream](https://github.com/moonlight-stream "moonlight-stream") project logo.
