import image
import sys
from pathlib import Path
import re


class Map:
    def __init__(self, x, y, filename):
        self.x = x
        self.y = y
        self.filename = filename

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: {} <input directory>".format(sys.argv[0]))
        sys.exit()

    directory = sys.argv[1]
    pathlist = Path(directory).glob('*.TGA')
    maps = []

    max_x = 0
    max_y = 0

    for path in pathlist:
        m = re.search("([0-9a-fA-F]{2})([0-9a-fA-F]{2})\\.TGA", str(path))
        pos_x = int(m.group(1), base=16)
        pos_y = int(m.group(2), base=16)
        map = Map(pos_x - 1, pos_y - 1, str(path))
        maps.append(map)
        if max_x < pos_x:
            max_x = pos_x
        if max_y < pos_y:
            max_y = pos_y

    image_width = 320
    image_height = 200

    world_map = image.Image(max_x * image_width, max_y * image_height)
    for map in maps:
        map_image = image.Image(1,1)
        map_image.loadFromTga(map.filename)
        world_map.drawImage(map_image, map.x * image_width, map.y * image_height)
    
    world_map.saveAsTga("world.tga")