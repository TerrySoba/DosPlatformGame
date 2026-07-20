#!/bin/bash

set -e

# Check if at least two parameters are provided
if [ $# -lt 2 ]; then
    echo "Copies files from the first partition of a hard drive image to the local directory."
    echo "Usage: $0 <harddrive_file> <file_to_be_copied_from_harddrive> [additional_files...]"
    exit 1
fi

harddrive_file=$1
shift

# echo "Using harddrive file: $harddrive_file"


for file in "$@"; do
    echo "Copying \"$file\" from \"$harddrive_file\"..."
    guestfish --ro -a "$harddrive_file" -m /dev/sda1 copy-out "/$file" .
done

echo
echo "Finished. Copied $# item(s) from \"$harddrive_file\"."
