#!/bin/bash

set -e

mkdir /build
cd /build
cmake -DCMAKE_TOOLCHAIN_FILE=/src/build/sdl/mingw-toolchain.cmake /src/build/sdl
make -j4
make install
cd /
zip -r game.zip /build/bin
cp game.zip /src/result/