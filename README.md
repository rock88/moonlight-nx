# Moonlight-libretro

Moonlight-libretro is a port of [Moonlight Game Streaming Project](https://github.com/moonlight-stream "Moonlight Game Streaming Project") for [RetroArch](https://www.retroarch.com "RetroArch") platform.
In the first place it's development for use on a Nintendo Switch, for use with [Lakka on the Nintendo Switch](https://lakka-switch.github.io/documentation/ "Lakka on the Nintendo Switch"), but can be ported on other platform (currently can run on macOS, for example).

# Installing (Lakka-Switch)
1. Install [Lakka](https://lakka-switch.github.io/documentation/installation.html "Lakka")
2. Download latest Moonlight-libretro [release](https://github.com/rock88/moonlight-libretro/releases "release")
3. Put moonlight_libretro.so to lakka/storage/cores directory on your SD card (create dir if it not exist).
4. Choose Load Core and select moonlight_libretro.so, then push Start Core.

# Controls (Lakka-Switch)
1. Move cursor with move finger on touch screen;
2. L/R + tap on screen - Left/Right mouse click (allow to move cursor);
3. ZL/ZR + tap on screen - Left/Right mouse click (without move cursor);
4. L+R+Down - exit from stream (and close current app);
5. L+R+Up - exit from stream (without closing current app);
6. L+R+Left - Alt+Enter (for enable/disable fullscreen mode);
7. L+R+Right - ESC key;
8. ZL+ZR+Left - show video decoder/render stats;
9. ZL+ZR+Right - hide video decoder/render stats;

# Building (Lakka-Switch)
1. Firstly needs build toolchain from [Lakka on the Nintendo Switch](https://github.com/lakka-switch/Lakka-LibreELEC)
2. Clone this repo `git clone --recursive https://github.com/rock88/moonlight-libretro.git`
3. `cd moonlight-libretro`
4. Fix `TOOLCHAIN` path in Makefile, build_opus_lakka_switch.sh and build_ffmpeg_lakka_switch.sh
5. `./build_opus_lakka_switch.sh`
6. (Optional - build latest ffmeg libs) `./build_ffmpeg_lakka_switch.sh`
7. `make platform=lakka-switch`
