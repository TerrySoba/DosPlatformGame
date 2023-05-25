#!/bin/bash

# This script is used to run the correct dosbox executable depending on the runtime.
# Supported are WSL2 and native Linux

#  check number of parameters
if [ $# -lt 2 ]; then
    echo "Usage: $0 CONFIG_FILE C_DRIVE [command1] [command2] ..."
    exit 1
fi


WSL_DOSBOX_PATH="/mnt/c/Program Files (x86)/DOSBox-0.74-3/DOSBox.exe"
LINUX_DOSBOX_PATH="dosbox"

COMMANDS=()
for i in "${@: 3}"; do
    COMMANDS+=("-c")
    COMMANDS+=("$i")
done

# check if we run on wsl (Windows Subsystem for Linux)
if [[ $(grep WSL /proc/version) ]]; then
    # we run on WSL
    CONFIG_PATH=$(wslpath -am $1)
    C_DRIVE=$(wslpath -am $2)
    
    "$WSL_DOSBOX_PATH" -conf "$CONFIG_PATH" -c "mount c $C_DRIVE" -c "c:" "${COMMANDS[@]}"
else
    # we run on native Linux
    $LINUX_DOSBOX_PATH -conf $1 -c "mount c $2" -c "c:" "${COMMANDS[@]}"
fi
