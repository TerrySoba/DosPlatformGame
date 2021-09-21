import os
import re



files = []

for filename in os.listdir("."):
    m = re.fullmatch("([0-9]{2})([0-9]{2})\.tmx", filename)
    if m:
        x = int(m.group(1))
        y = int(m.group(2))
        files.append([x,y,filename])

desc = sorted(files, key=lambda item: -item[0])
asc = sorted(files, key=lambda item: item[0])

for file in desc:
    os.rename(file[2], "{:02}{:02}.tmx".format(file[0] + 1, file[1]))
    # print("rename:{} to:{:02}{:02}.tmx".format(file[2], file[0] + 1, file[1]))