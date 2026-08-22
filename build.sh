#!/bin/bash

set -e


if [ $(uname -m) = "aarch64" ]
then
    # Install amd64 emulation using qemu, so that the build may also run on
    # non x86 platforms like arm.
    docker run --privileged --rm tonistiigi/binfmt --install 386
fi

# ./create_docker_image.sh

# rm -f source/*.o source/*.exe
# docker run --user $(id -u):$(id -g) -v `pwd`/source/:/build open_watcom /build
./ci_build.sh
rm -rf release && mkdir release &&
cat source/distfiles.txt | xargs -I FILENAME cp source/FILENAME release

# append git hash to readme.txt
echo `git rev-parse HEAD` >> release/readme.txt

mkdir -p zip

rm -f zip/game.zip
zip -r -9 zip/game.zip release

# create self extracting lha archive

rm -f zip/gamesfx.exe zip/game.lzh

pushd release
lha a -o ../zip/game.lzh *.*
popd
cat tools/dos/lha_sfx_stub zip/game.lzh > zip/gamesfx.exe


# dosbox \
#     --conf tools/dos/lharc.dosbox_config \
#     -c "mount c `pwd`/tools/dos" \
#     -c "mount d `pwd`/zip" \
#     -c "mount e `pwd`/release" \
#     -c "d:" \
#     -c "dir" \
#     -c "c:\\lharc s d:\\gameinst.lzh" 



    #-c "c:\\lharc a d:\\gameinst.lzh e:\\*.*" \





