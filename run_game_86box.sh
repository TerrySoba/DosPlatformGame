#!/bin/bash

EMULATOR_EXECUTABLE=~/Downloads/86Box-Linux-x86_64-b8000.AppImage
ROM_PATH=~/emu/86box/roms-5.2
VM_PATH=./86box_vm/Dos286
CONFIG_TEMPLATE=86box.cfg.template
TARGET_CFG=86box.cfg
awk -v p="$PWD" '{ gsub(/\$PROJECT_BASE_DIR\$/, p); print }' "$VM_PATH/$CONFIG_TEMPLATE" > "$VM_PATH/$TARGET_CFG"

$EMULATOR_EXECUTABLE --rompath $ROM_PATH --vmpath $VM_PATH