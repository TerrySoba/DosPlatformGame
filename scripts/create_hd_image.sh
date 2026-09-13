#!/bin/bash

set -e

script_dir="$(dirname "$0")"
hd_template_dir="$script_dir"/../hd_template
artifacts_dir="$script_dir"/../artifacts

echo "script_dir: $script_dir"

# xzcat "$hd_template_dir"/dos_svar.img.xz > "$script_dir"/hd_image.img

# xzcat "$hd_template_dir"/freedos_hd_template.img.xz > "$script_dir"/hd_image.img

image_file=$artifacts_dir/hd_image.VHD

# xzcat "$hd_template_dir"/svardos_template.VHD.xz > "$image_file"
# xzcat "$hd_template_dir"/msdos622_ger_template.vhd.xz > "$image_file"
xzcat "$hd_template_dir"/freedos_template.VHD.xz > "$image_file"

# AUTOEXEC_FILENAME=autoexec.bat
AUTOEXEC_FILENAME=fdauto.bat

"$hd_template_dir"/copy_from_first_partition.sh "$image_file" "$AUTOEXEC_FILENAME"

printf 'set BLASTER=A220 I7 D1\r\n' >> "$AUTOEXEC_FILENAME"
# printf 'cd release\r\ngame --german\r\n' >> "$AUTOEXEC_FILENAME"
"$hd_template_dir"/copy_to_first_partition.sh "$image_file" "$AUTOEXEC_FILENAME"
rm "$AUTOEXEC_FILENAME"

"$hd_template_dir"/copy_to_first_partition.sh "$image_file" release
"$hd_template_dir"/copy_to_first_partition.sh "$image_file" "$hd_template_dir"/game.bat
