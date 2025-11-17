#!/bin/bash

set -e

file_position=$(realpath "$0")
file_position=$(dirname "$file_position")

echo "file_position: $file_position"

xzcat $file_position/hd_template/dos_svar.img.xz > $file_position/hd_image.img

$file_position/hd_template/copy_from_first_partition.sh $file_position/hd_image.img autoexec.bat

printf 'set BLASTER=A220 I5 D1\r\n' >> autoexec.bat
printf 'cd release\r\ngame2.exe\r\n' >> autoexec.bat
$file_position/hd_template/copy_to_first_partition.sh $file_position/hd_image.img autoexec.bat
rm autoexec.bat

$file_position/hd_template/copy_to_first_partition.sh $file_position/hd_image.img release
$file_position/hd_template/copy_to_first_partition.sh $file_position/hd_image.img $file_position/hd_template/game.bat
