
class _OpenMode:
    def __init__(self, lib):
        self.Read = lib.enumOpenMode_Read()
        self.Write = lib.enumOpenMode_Write()
        self.Append = lib.enumOpenMode_Append()

class _CompressMode:
    def __init__(self, lib):
        self.NoCompress = lib.enumCompressMode_NoCompress()
        self.FastCompress = lib.enumCompressMode_FastCompress()
        self.MediumCompress = lib.enumCompressMode_MediumCompress()
        self.SlowCompress = lib.enumCompressMode_SlowCompress()
