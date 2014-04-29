
from ctypes import *
from ctypes import util
import os
import sys

from . import enums

# find and load the library
libfile = util.find_library('_libgetar')

# if not found, try relative to the module path
if libfile is None:
    path = os.path.dirname(__file__)
    libfile = os.path.join(path, '_libgetar.so')
    if not os.path.isfile(libfile):
        raise RuntimeError('_libgetar.so not found')

_libgetar = CDLL(libfile, use_errno=True);

OpenMode = enums._OpenMode(_libgetar)
CompressMode = enums._CompressMode(_libgetar)

_libgetar.newGTAR.argtypes = [c_char_p, c_uint]
_libgetar.newGTAR.restype = c_void_p
newGTAR = _libgetar.newGTAR

_libgetar.delGTAR.argtypes = [c_void_p]
_libgetar.delGTAR.restype = None
delGTAR = _libgetar.delGTAR

_libgetar.writePtr.argtypes = [c_void_p, c_char_p, c_void_p, c_uint, c_uint]
_libgetar.writePtr.restype = None
writePtr = _libgetar.writePtr

_libgetar.readBytes.argtypes = [c_void_p, c_char_p, c_uint]
_libgetar.readBytes.restype = c_char_p
readBytes = _libgetar.readBytes

# helper function to convert a str to a char_p array for use in ctypes
def _str_to_char_p(s):
    # return the same thing if it is already a bytes object
    if isinstance(s, bytes):
        return s;

    # need to convert to a bytes object, which is different in python2 and python3
    if sys.version_info[0] > 2:
        return bytes(s, 'utf_8');
    else:
        return bytes(s);
