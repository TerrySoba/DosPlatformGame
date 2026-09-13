#!/bin/bash

script_dir="$(dirname "$0")"
artifacts_dir="$script_dir"/../artifacts

sfx_archive_name=gamesfx.exe
installer_dir="$script_dir"/../installer

rm -f "$installer_dir/$sfx_archive_name"
mkdir -p "$installer_dir"

cp "$artifacts_dir/$sfx_archive_name" "$installer_dir/$sfx_archive_name"
