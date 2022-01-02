import json, sys, struct

def readFile(path):
    with open(path, "rb") as file:
        return file.read()



if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: {} <input json> <output anim>".format(sys.argv[0]))
        sys.exit()
    
    jsonFile = readFile(sys.argv[1])
    contents = json.loads(jsonFile)
    
    frameNumber = len(contents["frames"])
    tagNumber = len(contents["meta"]["frameTags"])

    with open(sys.argv[2], "wb") as output:
        output.write(b"ANIM")
        output.write(struct.pack("<H", frameNumber))

        for filename in contents["frames"]:
            frame = contents["frames"][filename]
            x = frame["frame"]["x"]
            y = frame["frame"]["y"]
            w = frame["frame"]["w"]
            h = frame["frame"]["h"]
            output.write(struct.pack("<HHHH", x, y, w, h))

        output.write(struct.pack("<H", tagNumber))
        for frameTag in contents["meta"]["frameTags"]:
            output.write(struct.pack("<HH", frameTag["from"], frameTag["to"]))
            name = frameTag["name"]
            output.write(struct.pack("<H", len(name)))
            output.write(name.encode("ASCII"))

