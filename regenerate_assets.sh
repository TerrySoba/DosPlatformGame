#!/bin/bash

set -e


# build png to tga converter
mkdir -p Png2Tga/build
pushd Png2Tga/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
popd
png2tga=Png2Tga/build/Png2Tga


# build TextCompiler
mkdir -p TextCompiler/build
pushd TextCompiler/build
cmake ..
make
popd
textcompiler=TextCompiler/build/TextCompiler

# now convert text json to binary
$textcompiler text/strings.json text/strings


# build AdpcmEncoder
mkdir -p AdpcmEncoder/build
pushd AdpcmEncoder/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
popd
wav2adpcm=AdpcmEncoder/build/adpcm_encoder

$wav2adpcm -i sound/death.wav -f 8000 -c ADPCM4 -o sound/death.voc -n 0.7
$wav2adpcm -i sound/jump.wav -f 8000 -c ADPCM4 -o sound/jump.voc -n 0.2
$wav2adpcm -i sound/steps.wav -f 8000 -c ADPCM4 -o sound/steps.voc -n 0.2
$wav2adpcm -i sound/stand.wav -f 8000 -c ADPCM4 -o sound/stand.voc -n 0.2
$wav2adpcm -i sound/guffin.wav -f 8000 -c ADPCM4 -o sound/guffin.voc -n 0.8
$wav2adpcm -i sound/jetpack.wav -f 8000 -c ADPCM4 -o sound/jetpack.voc -n 0.9
$wav2adpcm -i sound/switch2.wav -f 11000 -c ADPCM4 -o sound/switch2.voc -n 0.9


convertImage () {
    local pngfile=$1
    directory=`dirname $pngfile`
    filename=`basename $pngfile .png`
    echo Converting image \"${directory}/${filename}.png\" to \"${directory}/${filename}.tga\"
    $png2tga -c ${directory}/${filename}.png ${directory}/${filename}.tga
}

# now convert png images to tga
for pngfile in images/*.png; do  convertImage "$pngfile" ; done

convertAnimation () {
    local jsonfile=$1
    directory=`dirname $jsonfile`
    filename=`basename $jsonfile .json`
    echo Converting animation \"${directory}/${filename}.json\" to .ani
    python3 AnimationTool/animation_tool.py ${directory}/${filename}.json ${directory}/${filename}.ani
}

# now animations
for jsonfile in images/*.json; do  convertAnimation "$jsonfile" ; done

# generate level data
pushd levels
make -j
popd

