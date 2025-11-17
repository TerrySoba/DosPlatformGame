#!/bin/bash
set -euo pipefail


./build.sh
./create_hd_image.sh
./run_game_86box.sh
