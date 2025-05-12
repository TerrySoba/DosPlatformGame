#!/bin/bash

set -e


# build png to tga converter
mkdir -p Png2Tga/build
pushd Png2Tga/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
popd
png2tga=Png2Tga/build/Png2Tga


# build TextCompiler
mkdir -p TextCompiler/build
pushd TextCompiler/build
cmake ..
make -j
popd
textcompiler=TextCompiler/build/TextCompiler

# now convert text json to binary
$textcompiler text/strings.json text/strings


## build VocTool

# if no CMakelists.txt is present, we need output an error message
if [ ! -f VocTool/CMakeLists.txt ]; then
    echo "Submodule VocTool is not present. You should run \"git submodule update --init --recursive\" to fetch it."
    exit 1
fi

mkdir -p VocTool/build
pushd VocTool/build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
popd
wav2adpcm=VocTool/build/voctool
encodeopus="opusenc --bitrate 128"

encodevorbis="oggenc --resample 48000 -b 128"


$wav2adpcm -i sound/death.wav -f 8000 -c ADPCM4 -o sound/death.voc -n 0.7
$wav2adpcm -i sound/jump.wav -f 8000 -c ADPCM4 -o sound/jump.voc -n 0.2
$wav2adpcm -i sound/steps.wav -f 8000 -c ADPCM4 -o sound/steps.voc -n 0.2
$wav2adpcm -i sound/stand.wav -f 8000 -c ADPCM4 -o sound/stand.voc -n 0.2
$wav2adpcm -i sound/guffin.wav -f 8000 -c ADPCM4 -o sound/guffin.voc -n 0.5
# $wav2adpcm -i sound/jetpack.wav -f 8000 -c ADPCM2 -o sound/jetpack.voc -n 0.9 -l 10
$wav2adpcm -i sound/switch2.wav -f 11000 -c ADPCM4 -o sound/switch2.voc -n 0.9

$encodeopus sound/death.wav sound/death.opus
$encodeopus sound/jump.wav sound/jump.opus
$encodeopus sound/steps.wav sound/steps.opus
$encodeopus sound/stand.wav sound/stand.opus
$encodeopus sound/guffin.wav sound/guffin.opus
$encodeopus sound/jetpack.wav sound/jetpack.opus
$encodeopus sound/switch2.wav sound/switch2.opus

echo lala 123

$encodevorbis sound/death.wav -o sound/death.ogg
$encodevorbis sound/jump.wav -o sound/jump.ogg
$encodevorbis sound/steps.wav -o sound/steps.ogg
$encodevorbis sound/stand.wav -o sound/stand.ogg
$encodevorbis sound/guffin.wav -o sound/guffin.ogg
$encodevorbis sound/jetpack.wav -o sound/jetpack.ogg
$encodevorbis sound/switch2.wav -o sound/switch2.ogg

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

