#!/bin/bash

set -e

pacman -Sy

# Tools for building the dos variant
REQUIRED_PACKAGES="git gcc cmake nasm python3 zip mtools catch2 upx"

# Tools for building the SDL port
REQUIRED_PACKAGES="$REQUIRED_PACKAGES sdl3 ninja vorbis-tools"

pacman -S $REQUIRED_PACKAGES
