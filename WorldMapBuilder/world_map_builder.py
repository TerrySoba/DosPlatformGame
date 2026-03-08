import image
import sys
from pathlib import Path
import re
import struct


class Map:
    def __init__(self, x, y, filename):
        self.x = x
        self.y = y
        self.filename = filename
        self.music_index = 0

def read_music_index(map_filename):
    """Read the music index from a level map file."""
    LAYER_MUSIC = 9  # Must match the LayerType enum in level.cpp
    
    try:
        with open(map_filename, 'rb') as f:
            # Read and verify header
            header = f.read(3)
            if header != b'MAP':
                return 0
            
            # Read layer count
            layer_count_bytes = f.read(2)
            if len(layer_count_bytes) < 2:
                return 0
            layer_count = struct.unpack('<H', layer_count_bytes)[0]
            
            # Search for LAYER_MUSIC
            for i in range(layer_count):
                layer_type_bytes = f.read(1)
                if len(layer_type_bytes) < 1:
                    return 0
                layer_type = struct.unpack('B', layer_type_bytes)[0]
                
                layer_data_size_bytes = f.read(2)
                if len(layer_data_size_bytes) < 2:
                    return 0
                layer_data_size = struct.unpack('<H', layer_data_size_bytes)[0]
                
                if layer_type == LAYER_MUSIC:
                    # Read the music index
                    music_index_bytes = f.read(2)
                    if len(music_index_bytes) >= 2:
                        return struct.unpack('<H', music_index_bytes)[0]
                    return 0
                else:
                    # Skip this layer's data
                    f.read(layer_data_size)
    except:
        pass
    
    return 0

music_index_mapping = {
    0: "MUSIC_INDEX_NO_CHANGE",  
    1: "MUSIC_INDEX_NO_MUSIC",  
    2: "MUSIC_INDEX_MAIN_MUSIC",  
    3: "MUSIC_INDEX_DARK_MUSIC",
    4: "MUSIC_INDEX_BOSS_MUSIC",  
    5: "MUSIC_INDEX_DIGITAL_MUSIC",
    6: "MUSIC_INDEX_BOSS2_MUSIC",
    7: "MUSIC_INDEX_STRANGE_MUSIC",
    8: "MUSIC_INDEX_INTRO_MUSIC",
    9: "MUSIC_INDEX_MYSTIC_MUSIC",
}

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
        
        # Try to read the corresponding map file to get music index
        map_file = str(path).replace('.TGA', '.map').replace('.tga', '.map')
        map.music_index = read_music_index(map_file)
        
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
        
        # Draw the music index in the top-right corner of the level image
        # music_text = music_index_mapping[int(map.music_index)]
        music_text = "MUSIC  {}".format(music_index_mapping[map.music_index] ,map.music_index)
        text_x = map.x * image_width + 10
        text_y = map.y * image_height + 10
        world_map.drawText(music_text, text_x, text_y, color_index=15)
    
    world_map.saveAsTga("world.tga")