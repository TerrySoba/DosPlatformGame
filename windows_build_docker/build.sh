#!/bin/bash

docker build -t w64build .
docker run -v `pwd`/../:/src -it w64build