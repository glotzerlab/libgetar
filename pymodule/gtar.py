from ctypes import *
from . import libgetar
from libgetar import _str_to_char_p

class GTAR:
    openModeDict = {'r': libgetar.OpenMode.Read,
                    'w': libgetar.OpenMode.Write,
                    'a': libgetar.OpenMode.Append}

    def __init__(self, filename, mode):
        try:
            self.mode = mode
            self.cmode = self.openModeDict[mode]
        except KeyError: # wasn't found in the dictionary
            raise RuntimeError('Unknown GTAR open mode: {}'.format(mode))

        self.filename = filename
        self.cgtar = libgetar.newGTAR(_str_to_char_p(filename), self.cmode)

    def __del__(self):
        libgetar.delGTAR(self.cgtar)
        self.cgtar = None

    def writeBytes(self, path, contents, compressMode):
        cpath = _str_to_char_p(path)
        cconts = _str_to_char_p(contents)

        libgetar.writePtr(self.cgtar, cpath, cconts, len(cconts), compressMode)

    def readBytes(self, path):
        size = pointer(c_uint(0))
        result = libgetar.readBytes(self.cgtar, _str_to_char_p(path), size)

        return string_at(result, size.contents.value)
