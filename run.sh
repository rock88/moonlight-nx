#!/bin/sh

make -f Makefile.libnx
/opt/devkitpro/tools/bin/nxlink -s moonlight.nro
