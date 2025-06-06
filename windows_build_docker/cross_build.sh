#!/bin/bash

set -e

mkdir -p /build
cd /build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=/src/build/sdl/mingw-toolchain.cmake /src/build/sdl
make -j8
make install
strip bin/*.dll
strip bin/*.exe
cd /
zip -r game.zip /build/bin
mkdir -p /src/result
cp game.zip /src/result/