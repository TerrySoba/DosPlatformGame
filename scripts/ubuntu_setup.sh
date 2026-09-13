#!/bin/bash
apt update

# Tools for building the dos variant
REQUIRED_PACKAGES="git gcc g++ cmake nasm python3 zip p7zip-full mtools catch2"

# Tools for building the SDL port
REQUIRED_PACKAGES="$REQUIRED_PACKAGES ninja-build libsdl3-dev vorbis-tools"

if [[ "$(uname -m)" = "x86_64" || "$(uname -m)" = "i686" ]]; then
    REQUIRED_PACKAGES="$REQUIRED_PACKAGES upx"
fi

if [[ "$(uname -m)" = "aarch64" || "$(uname -m)" = "armv7l" ]]; then
    REQUIRED_PACKAGES="$REQUIRED_PACKAGES docker.io dosbox"
fi

apt -y install $REQUIRED_PACKAGES
