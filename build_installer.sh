#!/bin/bash

ARCHIVE_NAME=game.yar
DIR_NAME=installer

rm $DIR_NAME/$ARCHIVE_NAME
mkdir $DIR_NAME


if [ "$(uname -m)" = "x86_64" ]
then
    COMPRESSOR_EXECUTABLE=tools/x86_64/compressor
fi

if [ "$(uname -m)" = "aarch64" ]
then
    COMPRESSOR_EXECUTABLE=tools/arm64/compressor
fi

$COMPRESSOR_EXECUTABLE release/* $DIR_NAME/$ARCHIVE_NAME
