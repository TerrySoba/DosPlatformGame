import argparse, copy, struct


def loadFile(filename):
    with open(filename, "rb") as file:
        return file.read()


def storeFile(filename, data):
    with open(filename, "wb") as file:
        file.write(data)


def clamp(n, smallest, largest):
    return max(smallest, min(n, largest))


scaleMap = [
        [0,  1,  2,  3,  4,  5,  6,  7], 
        [1,  3,  5,  7,  9, 11, 13, 15],
        [2,  6, 10, 14, 18, 22, 26, 30], 
        [4, 12, 20, 28, 36, 44, 52, 60]
]

adjustMap = [
 [ 0, 0, 0, 0, 0, 1, 1, 1],
 [-1, 0, 0, 0, 0, 1, 1, 1],
 [-1, 0, 0, 0, 0, 1, 1, 1],
 [-1, 0, 0, 0, 0, 0, 0, 0]
]

class CreativeAdpcmDecoder4Bit:
    def __init__(self, firstValue):
        self.scale = 0
        self.reference = firstValue
        pass

    def decodeNibble(self, nibble):
        negativeBit = nibble & (1 << 3)
        sample = nibble & ((1 << 3) - 1)

        sign = -1 if negativeBit else 1

        ref = self.reference + (sign * scaleMap[self.scale][sample])
        self.reference = clamp(ref, 0, 255)
        self.scale = self.scale + adjustMap[self.scale][sample]

        return self.reference


def createAdpcm4BitFromRaw(raw):

    decoder = CreativeAdpcmDecoder4Bit(raw[0])

    result = []

    for sample in raw[1:]:
        decoders = []
        diffs = []

        # print(sample)

        for i in range(16):
            decoderCopy = copy.deepcopy(decoder)
            diff = abs(decoderCopy.decodeNibble(i) - sample)
            decoders.append(decoderCopy)
            diffs.append(diff)

        # now find best one
        val, idx = min((val, idx) for (idx, val) in enumerate(diffs))

        decoder = decoders[idx]
        result.append(idx)
        # print(idx)
            
    binaryResult = []

    # merge nibbles into bytes
    for i in range(len(result) // 2):
        binaryResult.append((result[2 * i] << 4) + (result[2 * i + 1]))

    return struct.pack("B", raw[0]) + bytes(binaryResult)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert raw 8bit unsigned sound files to Creative ADPCM')
    parser.add_argument('input', help='The input raw audio file (8bit unsigned)')
    parser.add_argument('output', help='The ADPCM encoded audio file')
    parser.add_argument('frequency', type=int, help="The frequency of the sound in Hz, e.g. 11000")

    args = parser.parse_args()
    print(args)
    raw = loadFile(args.input)

    # version = 0x0aff
    # versionCheck = 0x1234

    compressed = createAdpcm4BitFromRaw(raw)

    timeConstant = round(256 - 1000000 / args.frequency)

    vocHeader = b"Creative Voice File" + struct.pack("bbbbbbb", 0x1a, 0x1a, 0x00, 0x0a, 0x01, 0x29, 0x11) # , version) #, (~version + versionCheck))
    sampleHeader = b"\x01" + struct.pack("I", len(compressed) + 2)[:3] + struct.pack("BB", timeConstant, 1)
    endheader = b"\x00"

    storeFile(args.output, vocHeader + sampleHeader + compressed + endheader)