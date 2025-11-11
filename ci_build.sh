#!/bin/bash
set -e


# load version file "version.txt" into environment variable VERSION
export BUILD_VERSION=$(cat version.txt)

# If GITHUB_RUN_NUMBER is set, replace last part of version with it
if [ ! -z "$GITHUB_RUN_NUMBER" ]; then
    export BUILD_VERSION="${BUILD_VERSION%.*}.$GITHUB_RUN_NUMBER"
fi

echo "Building version $BUILD_VERSION"

DIR=$(dirname $0)
cd $DIR
BASE_DIR=$(pwd)
mkdir -p dos_build
cd $BASE_DIR/dos_build

# ls -lh $BASE_DIR/open_watcom_docker/open_watcom.tar.gz
tar -xzf $BASE_DIR/open_watcom_docker/open_watcom.tar.gz


export WATCOM=$BASE_DIR/dos_build/opt/opwatcom
export PATH=$WATCOM/binl:$PATH
export EDPATH=$WATCOM/eddat
export INCLUDE=$WATCOM/h

cd $BASE_DIR/source

make -j5 -f $BASE_DIR/build/dos/Makefile
