#!/bin/bash
set -e

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
