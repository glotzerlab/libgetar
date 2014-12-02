import sys
import gtar
import numpy as np

def test_readWritePath():
    records = {'test.txt': 'test string foo\n',
               'blah.txt': '日本語',
               'frames/0/otherblah.f32.ind': np.random.rand(5),
               'frames/0/anotherblah.f64.ind': np.random.rand(7),
               'frames/0/number.i32.uni': 14}

    convertedRecords = {'frames/0/otherblah.f32.ind':
                        np.array(records['frames/0/otherblah.f32.ind'],
                                 dtype=np.float32)}

    with gtar.GTAR('test.zip', 'w') as arch:
        for path in records:
            arch.writePath(path, records[path])

    success = True
    with gtar.GTAR('test.zip', 'r') as arch:
        for path in records:
            equal = np.all(
                arch.readPath(path) == convertedRecords.get(path, records[path]))
            if not equal:
                print('Failed on readWritePath(): {}'.format(path))
            success = success and equal

    return success

def main():
    success = True

    success = success and test_readWritePath()

    sys.exit(not success)

if __name__ == '__main__': main()
