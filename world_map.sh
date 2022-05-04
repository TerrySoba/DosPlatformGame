#!/bin/sh

./build.sh
dosbox -conf run_game_create_world_map.dosbox_config
python3 WorldMapBuilder/world_map_builder.py release
convert world.tga world.png