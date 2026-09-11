#!/bin/bash

set -e

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file.lha> <directory to compress>"
    echo ""
    echo "Compresses the contents of the given directory into the specified .lzh file"
    echo "using dos lha 2.55"
    exit 1
fi

lha_directory="$(dirname "$0")"
echo "$lha_directory"

output_file="$1"
output_file_directory="$(dirname "$output_file")"

input_directory="$2"

# create temporary directory that will be deleted 
temp_directory="$(mktemp -d)"
trap 'rm -rf "$temp_directory"' EXIT

# now mount the lha directory and the input directory using dosbox and execute lha,
# using a config tuned for maximum cpu cycles / minimal overhead

SDL_VIDEODRIVER=dummy dosbox  \
        -conf "$lha_directory/lha.dosbox_config" \
        -c "mount c \"$lha_directory\"" \
        -c "mount d \"$input_directory\"" \
        -c "mount e \"$temp_directory\"" \
        -c "set PATH=c:\\" \
        -c "e:" \
        -c "lha a tmp.lzh d:\\*.*" \
        -c "lha s tmp.lzh" \
        -c "exit"

# move the resulting tmp.lzh to the desired output location
mv "$temp_directory/TMP.LZH" "$output_file"
mv "$temp_directory/TMP.EXE" "$(dirname "$output_file")/$(basename "$output_file" .lzh).exe"