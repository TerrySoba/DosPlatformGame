#!/bin/bash

set -e

pushd windows_build_docker
./build.sh
popd
