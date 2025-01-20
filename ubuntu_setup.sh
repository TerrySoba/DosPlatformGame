#!/bin/bash
apt update

REQUIRED_PACKAGES="git gcc g++ cmake nasm python3 zip p7zip-full mtools catch2"

if [[ "$(uname -m)" = "x86_64" || "$(uname -m)" = "i686" ]]; then
    REQUIRED_PACKAGES="$REQUIRED_PACKAGES upx"
fi

if [[ "$(uname -m)" = "aarch64" || "$(uname -m)" = "armv7l" ]]; then
    REQUIRED_PACKAGES="$REQUIRED_PACKAGES docker.io dosbox"
fi

apt -y install $REQUIRED_PACKAGES
