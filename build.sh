#!/bin/sh

set -e

# ./create_docker_image.sh

rm -f source/*.o source/*.exe

# docker run --user $(id -u):$(id -g) -v `pwd`/source/:/build open_watcom &&
./ci_build.sh
rm -rf release && mkdir release &&
cat source/distfiles.txt | xargs -I FILENAME cp source/FILENAME release

# append git hash to readme.txt
echo `git rev-parse HEAD` >> release/readme.txt

mkdir -p zip

rm -f zip/game.zip
zip -r zip/game.zip release


