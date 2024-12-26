import os, re

# Read the file 'distfile.txt' and put each line into an array

def read_distfile(file_path):
    with open(file_path, 'r') as file:
        lines = file.readlines()
    stripped_lines = [line.strip() for line in lines if line.strip()]
    return stripped_lines

def read_map_files(directory):
    files = []
    for filename in os.listdir(directory):
        m = re.fullmatch("(.*)\.tmx", filename)
        if m:
            files.append(m.group(1) + ".map")

    return files

distfile_path = '../source/distfiles.txt'
lines_array = read_distfile(distfile_path)

maps = read_map_files(".")
maps = ["../levels/" + map for map in maps]

# remove .map files from the distfile
lines_array = [line for line in lines_array if not line.endswith(".map")]
lines_array.extend(maps)

# sort the array
lines_array.sort()

# write the array back to the file
with open(distfile_path, 'w') as file:
    for line in lines_array:
        file.write(line + '\n')