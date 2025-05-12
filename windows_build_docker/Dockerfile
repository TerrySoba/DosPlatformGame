FROM ubuntu:25.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    cmake \
    make \
    mingw-w64 \
    mingw-w64-common \
    unzip \
    wget \
    && rm -rf /var/lib/apt/lists/*

# Download and install SDL3 for Windows
RUN wget https://github.com/libsdl-org/SDL/releases/download/release-3.2.12/SDL3-devel-3.2.12-mingw.tar.gz -O /tmp/SDL3.tar.gz \
    && mkdir -p /tmp/sdl3 \
    && tar -xzf /tmp/SDL3.tar.gz -C /tmp/sdl3 --strip-components=1 \
    && cd /tmp/sdl3 \
    && make install-x86_64 \
    && rm /tmp/SDL3.tar.gz \
    && rm -rf /tmp/sdl3

ENV CC=x86_64-w64-mingw32-gcc
ENV CXX=x86_64-w64-mingw32-g++

    