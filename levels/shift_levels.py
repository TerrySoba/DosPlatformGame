import os
import re
import argparse


def parse_input_rect(input_rect):
    m = re.fullmatch("([0-9A-Fa-f]{2})([0-9A-Fa-f]{2}):([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})", input_rect)
    if m:
        x1 = int(m.group(1), 16)
        y1 = int(m.group(2), 16)
        x2 = int(m.group(3), 16)
        y2 = int(m.group(4), 16)
        return x1, y1, x2, y2
    else:
        raise ValueError("Invalid input rectangle format")

def parse_output_pos(output_pos):
    m = re.fullmatch("([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})", output_pos)
    if m:
        x = int(m.group(1), 16)
        y = int(m.group(2), 16)
        return x, y
    else:
        raise ValueError("Invalid output position format")


parser = argparse.ArgumentParser(
    description='Shift levels in the levels directory',
    epilog='Example: python shift_levels.py --input-rect 0304:0405 --output-pos 1010')
parser.add_argument('-i', '--input-rect', required=True, help='Input rectangle in the format "0304:0405"', type=parse_input_rect)
parser.add_argument('-o', '--output-pos', required=True, help='Output position in the format "1010"', type=parse_output_pos)
args = parser.parse_args()

print("Input rectangle: {}".format(args.input_rect))
print("Output position: {}".format(args.output_pos))

x1, y1, x2, y2 = args.input_rect
outx, outy = args.output_pos

files_in_directory = os.listdir(".")  # Get all files in the current directory

direction = 1
if outy > y1:
    direction = -1
elif outy < y1:
    direction = 1
else: # outy == y1
    if outx > x1:
        direction = -1
    else:
        direction = 1

print("Direction: {}".format(direction))


def directable_range(start, end, direction):
    if direction == 1:
        return range(start, end + 1)
    else:
        return range(end, start - 1, -1)

for x in directable_range(x1, x2, direction):
    for y in directable_range(y1, y2, direction):
        filename_base = "{:02X}{:02X}".format(x, y)
        new_filename_base = "{:02X}{:02X}".format(outx + x - x1, outy + y - y1)

        for ext in ["b1.tmx", ".tmx"]:
            if filename_base + ext in files_in_directory:
                filename = filename_base + ext
                new_filename = new_filename_base + ext
                print("rename:{} to:{}".format(filename, new_filename))
                os.rename(filename, new_filename)

# exit()

# files = []

# for filename in os.listdir("."):
#     m = re.fullmatch("([0-9A-F]{2})([0-9A-F]{2})(b1)*\.tmx", filename)
#     if m:
#         x = int(m.group(1),16)
#         y = int(m.group(2),16)
#         files.append([x,y,filename])

# desc = sorted(files, key=lambda item: -item[0])
# asc = sorted(files, key=lambda item: item[0])

# for file in desc:
#     # os.rename(file[2], "{:02}{:02}.tmx".format(file[0] + 1, file[1]))
#     print("rename:{} to:{:02}{:02}.tmx".format(file[2], file[0] + 1, file[1]))