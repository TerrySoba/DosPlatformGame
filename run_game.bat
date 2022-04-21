set DOSBOX_EXE=C:\DOSBox-X\dosbox-x.exe

del release\game.sav
%DOSBOX_EXE% -fastlaunch -conf run_game_windows.dosbox_config
