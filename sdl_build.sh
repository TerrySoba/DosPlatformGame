#!/bin/bash

set -e

mkdir -p sdl_build
cd sdl_build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ../build/sdl
ninja
ninja install