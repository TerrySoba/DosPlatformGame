#!/bin/bash

ARCHIVE_NAME=game.yar
DIR_NAME=installer

rm $DIR_NAME/$ARCHIVE_NAME
mkdir $DIR_NAME
tools/compressor release/* $DIR_NAME/$ARCHIVE_NAME
