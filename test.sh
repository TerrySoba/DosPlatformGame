xvfb-run dosbox -c "mount c ." -c "c:" -c "source\\gametest.exe" -c exit
python3 parse_junit_xml.py
# dosbox -conf test.dosbox_config
# yes | dosemu -dumb source/gametest.exe

