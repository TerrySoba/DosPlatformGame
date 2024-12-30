#!/bin/bash

set -e

# Check if at least two parameters are provided
if [ $# -lt 2 ]; then
    echo "Copies files to the first partition of a hard drive image."
    echo "The harddrive file must contain a FAT16 partition."
    echo "Usage: $0 <harddrive_file> <file_to_be_added_to_harddrive> [additional_files]"
    exit 1
fi

harddrive_file=$1
shift

# echo "Using harddrive file: $harddrive_file"

# Check if harddrive_file is valid using fdisk
if ! fdisk -l "$harddrive_file" >/dev/null 2>&1; then
    echo "ERROR: Invalid harddrive file: $harddrive_file"
    exit 1
fi

filename_escaped=$(echo "$harddrive_file" | sed -e 's/[]\/$*.^|[]/\\&/g')

# Parse the start and end of partition from fdisk output
start=$(fdisk -l $harddrive_file | awk '/^'"$filename_escaped"'/ {print $3}')
end=$(fdisk -l $harddrive_file | awk '/^'"$filename_escaped"'/ {print $4}')

# echo "Start: $start"
# echo "End: $end"

# Create a temporary file
temp_partition_file=$(mktemp partition_XXXXXXXXXXXX.img)

# Ensure the temporary file is removed when the script exits
trap "rm -f $temp_partition_file" EXIT

# Extract the FAT16 partition into a file
dd if=$harddrive_file of=$temp_partition_file bs=512 skip=$start count=$(($end - $start + 1)) >/dev/null 2>&1

if ! file $temp_partition_file | grep -q "FAT"; then
    echo "ERROR: The partition file is not a FAT file system."
    file $temp_partition_file
    exit 1
fi

# Iterate over all parameters and copy the files to the image
for file in "$@"; do
    echo "Adding file: $file"
    # if file is a directory, copy all files in the directory
    if [ -d "$file" ]; then
        MTOOLS_NO_VFAT=1 mcopy -Q -i $temp_partition_file $file ::$file
    fi

    # if file is a regular file, copy the file
    if [ -f "$file" ]; then
        MTOOLS_NO_VFAT=1 mcopy -Q -i $temp_partition_file $file ::
    fi
done

# Reintegrate the partition into the disk image
dd if=$temp_partition_file of=$harddrive_file bs=512 seek=$start conv=notrunc  >/dev/null 2>&1

echo
echo "Finished. Copied $# item(s) to \"$harddrive_file\"."
