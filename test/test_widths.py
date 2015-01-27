import sys
import gtar
import numpy as np

def test_widths():
    with gtar.GTAR('test.zip', 'w') as arch:
        arch.writePath('position.f32.ind', np.random.rand(6))

    with gtar.GTAR('test.zip', 'r') as arch:
        posx3 = arch.readPath('position.f32.ind')

        gtar.widths['position'] = 2
        posx2 = arch.readPath('position.f32.ind')

        gtar.widths.clear()
        posFlat = arch.readPath('position.f32.ind')

    success = posx3.shape == (2, 3) and posx2.shape == (3, 2) and posFlat.shape == (6,)

    assert success
    return success

def main():
    success = test_widths()

    sys.exit(not success)

if __name__ == '__main__': main()
