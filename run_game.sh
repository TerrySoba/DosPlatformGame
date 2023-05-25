#!/bin/bash

# check if we run on wsl (Windows Subsystem for Linux)
if [[ $(grep WSL /proc/version) ]]; then
    # we run on WSL
    ./dosbox_launcher.sh run_game_windows.dosbox_config release "loadfix -150 game2 --german" "exit"
else
    # we run on native Linux
    ./dosbox_launcher.sh run_game.dosbox_config release "loadfix -150 game2 --level 15 4 --english" "exit"
fi
