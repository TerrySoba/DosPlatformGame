#!/bin/bash

set -e

file_position=$(realpath "$0")
file_position=$(dirname "$file_position")

echo "file_position: $file_position"

xzcat $file_position/hd_template/dos622_template.img.xz > $file_position/hd_image.img

$file_position/hd_template/copy_to_first_partition.sh $file_position/hd_image.img release


