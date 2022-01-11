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
wav2adpcm=AdpcmEncoder/wav2adpcm.sh

# now convert wav sounds to adpcm compressed voc files
for wavfile in sound/*.wav
do
    echo Converting $wavfile
    $wav2adpcm "$wavfile" 11000 sound/`basename $wavfile .wav`.voc
done


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

generateCsv () {
    local tmxfile=$1
    directory=`dirname $tmxfile`
    filename=`basename $tmxfile .tmx`
    echo Converting level \"${directory}/${filename}.tmx\" to CSV
    python3 AnimationTool/map_tool.py ${directory}/${filename}.tmx
}

# now convert tiled maps to csv
for tmxfile in levels/*.tmx;
do
    generateCsv "$tmxfile"
done
