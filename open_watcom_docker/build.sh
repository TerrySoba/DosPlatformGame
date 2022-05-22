#!/bin/sh
workdir=$1
cd ${GITHUB_WORKSPACE}/source
make -j8
