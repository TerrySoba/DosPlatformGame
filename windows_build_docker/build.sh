#!/bin/bash

docker build -t w64build .
docker run -v `pwd`/../:/src -t w64build /cross_build.sh