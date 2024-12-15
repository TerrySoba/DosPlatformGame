import struct


rgbiColors = bytearray(
    b'\x00\x00\x00'
    b'\xAA\x00\x00'
    b'\x00\xAA\x00'
    b'\xAA\xAA\x00'
    b'\x00\x00\xAA'
    b'\xAA\x00\xAA'
    b'\x00\x55\xAA' 
    b'\xAA\xAA\xAA'
    b'\x55\x55\x55'
    b'\xFF\x55\x55'
    b'\x55\xFF\x55'
    b'\xFF\xFF\x55'
    b'\x55\x55\xFF'
    b'\xFF\x55\xFF'
    b'\x55\xFF\xFF'
    b'\xFF\xFF\xFF'
)

def write8bit(data, fp):
    fp.write(struct.pack("B", data))
    pass

def write16bit(data, fp):
    fp.write(struct.pack("<H", data))
    pass

class Image:
    def __init__(self, width, height):
        self._width = width
        self._height = height
        self._data = bytearray(width * height)
        pass

    def drawImage(self, img, x_target, y_target):
        for y in range(img._height):
            self_start = self._width * (y_target + y) + x_target
            other_start = y * img._width
            self._data[self_start:self_start + img._width] = img._data[other_start:other_start+img._width]


    def saveAsTga(self, filename):
        with open(filename, "wb") as fp:

            write8bit(0, fp) # id length in bytes
            write8bit(1, fp) # color map type (1 == color map / palette present)
            write8bit(1, fp) # image type (1 == uncompressed color-mapped image)
            write16bit(0, fp) # color map origin
            write16bit(16, fp) # color map length
            write8bit(24, fp) # color map depth (bits per palette entry)
            write16bit(0, fp) # x origin
            write16bit(0, fp) # y origin
            write16bit(self._width, fp) # image width
            write16bit(self._height, fp) # image height
            write8bit(8, fp) # bits per pixel
            write8bit((1 << 5), fp) # image descriptor

            # we do not have id data, so do not write any

            fp.write(rgbiColors)

            # write uncompressed
            fp.write(self._data)

    def loadFromTga(self, filename):
        with open(filename, "rb") as fp:
            format_string = "<BBBHHBHHHHBB"
            size = struct.calcsize(format_string)
            tga_header_data = fp.read(size)
            [
                idLength,
                colorMapType,
                imageType,
                colorMapOrigin,
                colorMapLength,
                colorMapDepth,
                xOrigin,
                yOrigin,
                width,
                height,
                bitsPerPixel,
                imageDescriptor
            ] = struct.unpack(format_string, tga_header_data)

            assert colorMapType == 1, "Only colorMapType 1 supported, but got: {}".format(colorMapType)
            assert imageType == 1, "Only imageTypes 1 is supported, but got: {}".format(imageType)
            assert bitsPerPixel == 8, "Only 8bits per pixel supported, but got: {}".format(bitsPerPixel)

            paletteEntrySize = 16 if colorMapDepth == 15 else colorMapDepth

            # we ignore id data, so seek over it
            fp.seek(idLength, 1) # seek relative

            # seek over color palette for now
            fp.seek((paletteEntrySize // 8) * colorMapLength, 1) # seek relative

            # load pixel data
            self._data = fp.read(width * height)
            
            self._width = width
            self._height = height


if __name__ == "__main__":
    pass