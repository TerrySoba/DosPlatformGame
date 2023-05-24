#!/bin/sh

./build.sh
./run_dosbox.sh run_game_create_world_map.dosbox_config release "game2 --dump-level-images" "exit"
python3 WorldMapBuilder/world_map_builder.py release
convert world.tga world.png