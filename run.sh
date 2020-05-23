#!/bin/sh

rm -fr moonlight.nro moonlight.elf moonlight.nacp
make -f Makefile -j4
/opt/devkitpro/tools/bin/nxlink -s moonlight.nro
