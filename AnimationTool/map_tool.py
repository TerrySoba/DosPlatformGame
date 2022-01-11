import sys, struct
import xml.etree.ElementTree as ET
import os.path



def readFile(path):
    with open(path, "rb") as file:
        return file.read()

class Tileset:
    def __init__(self, source, firstgid):
        self.source = source
        self.firstgid = firstgid
    
    def __str__(self):
        return "Tileset source:{} firstgid:{}".format(self.source, self.firstgid)
        
class Layer:
    def __init__(self, id, name, width, height, data):
        self.id = id
        self.name = name
        self.width = width
        self.height = height
        self.data = data

class DataMapper:
    def __init__(self, firstgids):
        self.firstgids = sorted(firstgids)

    def mapValue(self, value):
        if value == 0:
            return -1
        for index in range(len(self.firstgids) - 1):
            if value >= self.firstgids[index] and value < self.firstgids[index+1]:
                return value - self.firstgids[index]
        return value - self.firstgids[-1]

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: {} <input json> <output anim>".format(sys.argv[0]))
        sys.exit()

    data = readFile(sys.argv[1])
    root = ET.fromstring(data)
    # root = tree.getroot()
    # print(root.tag, root.attrib)

    tilesetElements = root.findall("./tileset")
    tilesets = []

    for tilesetElement in tilesetElements:
        tileset = Tileset(tilesetElement.attrib["source"], int(tilesetElement.attrib["firstgid"]))
        tilesets.append(tileset)
 
    layerElements = root.findall("./layer")
    layers = []

    mapper = DataMapper([tileset.firstgid for tileset in tilesets])

    for layerElement in layerElements:
        width = int(layerElement.attrib["width"])
        height = int(layerElement.attrib["height"])
        name = layerElement.attrib["name"]
        id = layerElement.attrib["id"]
        dataElements = layerElement.findall("./data")
        dataText = dataElements[0].text
        data = dataText.split(",")
        data = [int(x.strip()) for x in data]
        data = [mapper.mapValue(value) for value in data]

        layer = Layer(id, name, width, height, data)
        layers.append(layer)

    
    # output filename
    base_name = os.path.splitext(sys.argv[1])[0]


    for layer in layers:
        filename = "{}_{}.csv".format(base_name, layer.name)
        with open(filename, "w") as f:
            for y in range(layer.height):
                f.write(",".join([str(val) for val in layer.data[layer.width * y : layer.width * (y+1)]]) + "\n")


    