@echo off

REM Set the next line to the location of the "python.exe" file.
set PYTHON3_PATH="C:\Program Files\Python310\python.exe"

for %%f in (levels\*.tmx) do (
    echo Converting %%f
    %PYTHON3_PATH% AnimationTool\map_tool.py %%f
    copy levels\%%~nf.map release\%%~nf.map
)
