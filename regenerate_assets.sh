#!/bin/bash

set -e

# set this to the location of tiled
tiled_path=~/Downloads/Tiled-1.5.0-x86_64.AppImage

# build png to tga converter
mkdir -p Png2Tga/build
pushd Png2Tga/build
cmake ..
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


convertImage () {
    local pngfile=$1
    directory=`dirname $pngfile`
    filename=`basename $pngfile .png`
    echo Converting image \"${directory}/${filename}.png\" to \"${directory}/${filename}.tga\"
    $png2tga -c ${directory}/${filename}.png ${directory}/${filename}.tga
}

# now convert png images to tga
for pngfile in images/*.png; do  convertImage "$pngfile" & done

# cp images/guy.json images/guy.jsn
cp images/enemy.json images/enemy.jsn
cp images/guffin.json images/guffin.jsn

generateCsv () {
    local tmxfile=$1
    directory=`dirname $tmxfile`
    filename=`basename $tmxfile .tmx`
    echo Converting level \"${directory}/${filename}.tmx\" to CSV
    $tiled_path ${directory}/${filename}.tmx --export-map ${directory}/${filename}.csv
}

# now convert tiled maps to csv
for tmxfile in levels/*.tmx; do generateCsv "$tmxfile"; done
