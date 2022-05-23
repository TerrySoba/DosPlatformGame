#!/bin/sh

# if this is run on github runner, then prefix contents of GITHUB_WORKSPACE to workdir
if [ -z ${GITHUB_WORKSPACE+x} ]; then
    workdir=$1
else 
    workdir=$GITHUB_WORKSPACE/$1
fi

cd ${workdir}
make -j8
