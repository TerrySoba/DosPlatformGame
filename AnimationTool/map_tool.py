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

class ObjectLayer:
    def __init__(self, name, type, text_id, x, y, w, h):
        self.name = name
        self.type = type
        if text_id is not None:
            self.text_id = int(text_id)
        self.x = round(float(x))
        self.y = round(float(y))
        self.w = round(float(w))
        self.h = round(float(h))

class DataMapper:
    def __init__(self, firstgids):
        self.firstgids = sorted(firstgids)

    def mapValue(self, value):
        if value == 0:
            return 0
        for index in range(len(self.firstgids) - 1):
            if value >= self.firstgids[index] and value < self.firstgids[index+1]:
                return value - self.firstgids[index]
        return value - self.firstgids[-1]


def changeExtension(path, newExtension):
    return os.path.splitext(path)[0] + newExtension

def getTilesetImage(tilesetPath):
    """ Returns the filename of the tileset image. The extension is changed to .tga. """
    tilesetPath = os.path.join(os.path.dirname(sys.argv[1]), tilesetPath)
    tilesetXml = readFile(tilesetPath)
    root = ET.fromstring(tilesetXml)
    tilesetElements = root.find("./image")
    return changeExtension(os.path.basename(tilesetElements.attrib["source"]), ".tga")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: {} <input tmx file>".format(sys.argv[0]))
        sys.exit()

    data = readFile(sys.argv[1])
    root = ET.fromstring(data)

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


    objectElements = root.findall("./objectgroup/object")

    objectLayers = []

    for objectElement in objectElements:
        property = objectElement.find("./properties/property[@name='string_id']")
        try:
            textId = property.attrib["value"]
        except:
            textId = None

        objectLayers.append(ObjectLayer(
            objectElement.attrib["name"],
            objectElement.attrib["type"],
            textId,
            objectElement.attrib["x"],
            objectElement.attrib["y"],
            objectElement.attrib["width"],
            objectElement.attrib["height"]))


    # read guffin_gate property
    guffinGate = None
    guffinGateElement = root.find("./properties/property[@name='guffin_gate']")

    if guffinGateElement is not None:
        try:
            guffinGate = int(guffinGateElement.attrib["value"])
        except:
            pass

    # read music property
    music = None
    musicElement = root.find("./properties/property[@name='music']")

    if musicElement is not None:
        try:
            music = int(musicElement.attrib["value"])
        except:
            pass

    # read cutscene property
    cutscene = None
    cutsceneElement = root.find("./properties/property[@name='cutscene']")
    if cutsceneElement is not None:
        try:
            cutscene = int(cutsceneElement.attrib["value"])
        except:
            pass

    # read filename of tileset
    tilesetFilename = None
    tilesetFilenameElement = root.find("./tileset[@firstgid='1']")
    if tilesetFilenameElement is not None:
        tilesetXml = tilesetFilenameElement.attrib["source"]
        tilesetFilename = getTilesetImage(tilesetXml)

    # output filename
    base_name = os.path.splitext(sys.argv[1])[0]


    # This map needs to be kept in sync with the enum in level.cpp
    layerTypeMap = {
        "bg"          : 1,
        "col"         : 2,
        "text"        : 3,
        "fireball"    : 4,
        "seeker"      : 5,
        "guffin_gate" : 6,
        "boss1"       : 7,
        "play_time"   : 8,
        "music"       : 9,
        "boss2"       : 10,
        "tileset"     : 11,
        "eye"         : 12,
        "cutscene"    : 13,
    }

    with open(base_name + ".map", "wb") as map_file:
        map_file.write("MAP".encode("ascii"))           # map header

        layerCount = len(layers) + len(objectLayers)
        if guffinGate is not None:
            layerCount = layerCount + 1

        if music is not None:
            layerCount = layerCount + 1

        if cutscene is not None:
            layerCount = layerCount + 1

        if tilesetFilename is not None:
            layerCount = layerCount + 1

        map_file.write(struct.pack("<H", layerCount))  # no. of layers

        for objectLayer in objectLayers:
            layerData = bytearray()
            if objectLayer.type == "text":
                layerData += struct.pack("<H", objectLayer.text_id)
            layerData += struct.pack("<H", objectLayer.x)
            layerData += struct.pack("<H", objectLayer.y)
            layerData += struct.pack("<H", objectLayer.w)
            layerData += struct.pack("<H", objectLayer.h)
            try:
                map_file.write(struct.pack("B", layerTypeMap[objectLayer.type]))  # layer type
                map_file.write(struct.pack("<H", len(layerData)))       # size of layer
                map_file.write(layerData)
            except KeyError as e:
                print("WARNING: Found unknown object layer type {}.".format(e))
                pass


        for layer in layers:
            layerData = bytearray()
            layerData += struct.pack("<H", layer.width)
            layerData += struct.pack("<H", layer.height)
            layerData += bytes(bytearray(layer.data))
            map_file.write(struct.pack("B", layerTypeMap[layer.name]))  # layer type
            map_file.write(struct.pack("<H", len(layerData)))           # size of layer
            map_file.write(layerData)


        if guffinGate is not None:
            layerData = bytearray()
            layerData += struct.pack("<H", guffinGate)
            map_file.write(struct.pack("B", layerTypeMap["guffin_gate"]))  # layer type
            map_file.write(struct.pack("<H", len(layerData)))           # size of layer
            map_file.write(layerData)

        if music is not None:
            layerData = bytearray()
            layerData += struct.pack("<H", music)
            map_file.write(struct.pack("B", layerTypeMap["music"]))
            map_file.write(struct.pack("<H", len(layerData)))           # size of layer
            map_file.write(layerData)

        if cutscene is not None:
            layerData = bytearray()
            layerData += struct.pack("<H", cutscene)
            map_file.write(struct.pack("B", layerTypeMap["cutscene"]))
            map_file.write(struct.pack("<H", len(layerData)))           # size of layer
            map_file.write(layerData)

        if tilesetFilename is not None:
            filenameAscii = tilesetFilename.encode("ascii")
            layerData = bytearray()
            layerData += struct.pack("<H", len(filenameAscii))
            layerData += filenameAscii
            map_file.write(struct.pack("B", layerTypeMap["tileset"]))  # layer type
            map_file.write(struct.pack("<H", len(layerData)))           # size of layer
            map_file.write(layerData)

    