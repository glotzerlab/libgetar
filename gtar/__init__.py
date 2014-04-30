
from _gtar import *

def unPyify(obj):
    class Enum:
        pass
    result = Enum()
    vs = dict(vars(obj))
    # import pdb;pdb.set_trace()
    for key in vs:
        newkey = (key[2:] if key.startswith('Py') else key)
        setattr(result, newkey, vs[key])

    return result

OpenMode = unPyify(PyOpenMode)
CompressMode = unPyify(PyCompressMode)
Behavior = unPyify(PyBehavior)
Format = unPyify(PyFormat)
Resolution = unPyify(PyResolution)

GTAR = PyGTAR
