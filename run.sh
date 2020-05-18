#!/bin/sh

rm -fr moonlight.nro moonlight.elf moonlight.nacp
make -f Makefile.libnx
/opt/devkitpro/tools/bin/nxlink -s moonlight.nro
