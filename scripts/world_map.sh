#!/bin/sh

script_dir="$(dirname "$0")"
artifacts_dir="$script_dir"/../artifacts
cd "$script_dir"/..

./build.sh
./dosbox_launcher.sh "$script_dir"/run_game_create_world_map.dosbox_config release "game2 --dump-level-images" "exit"
python3 "$script_dir"/../WorldMapBuilder/world_map_builder.py release
mv "$script_dir"/../world.tga "$artifacts_dir"/world.tga
magick "$artifacts_dir"/world.tga "$artifacts_dir"/world.png