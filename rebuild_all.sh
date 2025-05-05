#!/bin/bash

set -e

./regenerate_assets.sh
./build.sh
./run_game_dos.sh