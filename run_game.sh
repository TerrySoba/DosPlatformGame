#!/bin/bash

# check if we run on wsl (Windows Subsystem for Linux)
if [[ $(grep WSL /proc/version) ]]; then
    # we run on WSL
    ./dosbox_launcher.sh run_game_windows.dosbox_config release "loadfix -100 game --german" "exit"
else
    # we run on native Linux
    ./dosbox_launcher.sh run_game.dosbox_config release "loadfix -100 game2 --level 1 1 --german"
fi
