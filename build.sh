#!/bin/bash

set -e

script_dir="$(dirname "$0")"
artifacts_dir="$script_dir"/artifacts

if [ $(uname -m) = "aarch64" ]
then
    # Install amd64 emulation using qemu, so that the build may also run on
    # non x86 platforms like arm.
    docker run --privileged --rm tonistiigi/binfmt --install 386
fi

# rm -f source/*.o source/*.exe
# docker run --user $(id -u):$(id -g) -v `pwd`/source/:/build open_watcom /build
./ci_build.sh
rm -rf release && mkdir release &&
cat source/distfiles.txt | xargs -I FILENAME cp source/FILENAME release

# append git hash to readme.txt
echo `git rev-parse HEAD` >> release/readme.txt

mkdir -p "$artifacts_dir"

rm -f "$artifacts_dir"/game.zip
echo "Creating zip archive..."
zip -q -r -9 "$artifacts_dir"/game.zip "$script_dir"/release

# create self extracting lha archive
rm -f "$artifacts_dir"/gamesfx.exe "$artifacts_dir"/game.lzh

tools/dos/lha.sh "$artifacts_dir"/game.lzh "$script_dir"/release
mv "$artifacts_dir"/game.exe "$artifacts_dir"/gamesfx.exe
