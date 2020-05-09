#!/bin/sh

/opt/devkitpro/devkitA64/bin/aarch64-none-elf-addr2line -e moonlight.elf -f -p -C -a $1
