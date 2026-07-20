#!/bin/bash

set -e

# Check if at least two parameters are provided
if [ $# -lt 2 ]; then
    echo "Copies files to the first partition of a hard drive image."
    echo "Usage: $0 <harddrive_file> <file_to_be_added_to_harddrive> [additional_files]"
    exit 1
fi

harddrive_file=$1
shift

# echo "Using harddrive file: $harddrive_file"

# Mount the harddrive image and copy files using guestfish
guestfish_cmds="run\nmount /dev/sda1 /\n"
for file in "$@"; do
    guestfish_cmds+="\ncopy-in \"$file\" /"
done
guestfish_cmds+="\nquit"

echo -e "$guestfish_cmds" | guestfish -a "$harddrive_file"

echo
echo "Finished. Copied $# item(s) to \"$harddrive_file\"."
