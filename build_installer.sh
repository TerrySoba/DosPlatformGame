#!/bin/bash

ARCHIVE_NAME=gamesfx.exe
DIR_NAME=installer

rm $DIR_NAME/$ARCHIVE_NAME
mkdir $DIR_NAME

cp zip/$ARCHIVE_NAME $DIR_NAME/$ARCHIVE_NAME
