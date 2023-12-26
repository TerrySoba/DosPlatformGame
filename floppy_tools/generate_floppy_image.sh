#!/bin/bash

# check if we got at least two parameters
if [ $# -lt 3 ]; then
    echo "Usage: $0 <image name> <file to be copied to disk image> <path to copy file to> [more files to be copied to disk image]]"
    exit 1
fi

IMAGE_FILE=$1
shift # remove first parameter from list

DISK_SECTOR_BYTES=512
DISK_HEADS=2
DISK_18_SECTORS=18
DISK_HD_TRACKS=80

DISK_1440_BYTES=`expr $DISK_SECTOR_BYTES \* $DISK_18_SECTORS \* $DISK_HD_TRACKS \* $DISK_HEADS` # 1474560

fallocate -l $DISK_1440_BYTES $IMAGE_FILE

mformat -i $IMAGE_FILE -v testdisk

# loop over each second element in parameter list
i=1
while [ $i -lt $# ]; do

    source_file=${!i}
    i=`expr $i + 1`
    target_path=${!i}
    i=`expr $i + 1`

    # get filename
    filename=`basename $target_path`

    # get dirname of file
    dir=`dirname $target_path`

    # if dir is . then empty string
    if [ "$dir" = "." ]; then
        dir=""
    else
        mmd -D s -i $IMAGE_FILE ::$dir
        dir="$dir/"
    fi

    echo "Copying '$source_file' to '$dir$filename'..."
    mcopy -i $IMAGE_FILE $source_file ::$dir$filename
done

echo "Successfully created 1.44MB disk image '$IMAGE_FILE'."

