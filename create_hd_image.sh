#!/bin/bash

set -e

file_position=$(realpath "$0")
file_position=$(dirname "$file_position")

echo "file_position: $file_position"

# xzcat $file_position/hd_template/dos_svar.img.xz > $file_position/hd_image.img

# xzcat $file_position/hd_template/freedos_hd_template.img.xz > $file_position/hd_image.img

image_file=$file_position/hd_image.VHD

# xzcat $file_position/hd_template/svardos_template.VHD.xz > "$image_file"
xzcat $file_position/hd_template/freedos_template.VHD.xz > "$image_file"

# AUTOEXEC_FILENAME=autoexec.bat
AUTOEXEC_FILENAME=fdauto.bat

$file_position/hd_template/copy_from_first_partition.sh "$image_file" "$AUTOEXEC_FILENAME"

printf 'set BLASTER=A220 I7 D1\r\n' >> "$AUTOEXEC_FILENAME"
printf 'cd release\r\ngame --german\r\n' >> "$AUTOEXEC_FILENAME"
$file_position/hd_template/copy_to_first_partition.sh "$image_file" "$AUTOEXEC_FILENAME"
rm "$AUTOEXEC_FILENAME"

$file_position/hd_template/copy_to_first_partition.sh "$image_file" release
$file_position/hd_template/copy_to_first_partition.sh "$image_file" "$file_position/hd_template/game.bat"
