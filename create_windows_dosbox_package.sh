#!/bin/bash

WINDOWS_GAME_DIR=DosPlatformGameWin32
WINDOWS_GAME_ZIP=DosPlatformGameWin32.zip

rm $WINDOWS_GAME_ZIP
rm -rf $WINDOWS_GAME_DIR
mkdir $WINDOWS_GAME_DIR
mkdir $WINDOWS_GAME_DIR/config
7z x tools/dosbox/dosbox-staging-win32.7z -y -o$WINDOWS_GAME_DIR
cp -r release $WINDOWS_GAME_DIR
cp run_game_windows.dosbox_config $WINDOWS_GAME_DIR/config
cp tools/dosbox/run_game_german.bat $WINDOWS_GAME_DIR
cp tools/dosbox/run_game_english.bat $WINDOWS_GAME_DIR
zip -r DosPlatformGameWin32.zip $WINDOWS_GAME_DIR
