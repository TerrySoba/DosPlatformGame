REM Set the next line to the location of the "tiled.exe" file.
set TILED_EXE="C:\Program Files\Tiled\tiled.exe"

for %%f in (levels\*.tmx) do (
    %TILED_EXE% %%f --export-map levels\%%~nf.csv
)
