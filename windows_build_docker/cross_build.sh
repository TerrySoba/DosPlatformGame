#!/bin/bash

set -e

mkdir -p /build
cd /build
cmake -DCMAKE_TOOLCHAIN_FILE=/src/build/sdl/mingw-toolchain.cmake /src/build/sdl
make -j4
make install
cd /
zip -r game.zip /build/bin
mkdir -p /src/result
cp game.zip /src/result/