import sys
import gtar
import numpy as np

def test_readThenWrite(suffix):
    records = {'test.txt': 'test string foo\n',
               'blah.txt': '日本語',
               'frames/0/otherblah.f32.ind': np.random.rand(5),
               'frames/0/anotherblah.f64.ind': np.random.rand(7),
               'frames/0/number.i32.uni': 14}

    convertedRecords = {'frames/0/otherblah.f32.ind':
                        np.array(records['frames/0/otherblah.f32.ind'],
                                 dtype=np.float32)}

    with gtar.GTAR('test' + suffix, 'w') as arch:
        for path in records:
            arch.writePath(path, records[path])

    success = True
    for mode in ['r', 'a']:
        with gtar.GTAR('test' + suffix, mode) as arch:
            for path in records:
                equal = np.all(
                    arch.readPath(path) == convertedRecords.get(path, records[path]))
                if not equal:
                    print('Failed on readThenWrite() with {} ({} mode): {}'.format(
                        suffix, mode, path))
                success = success and equal

    assert success
    return success

def test_readAndWrite(suffix):
    records = {'test.txt': 'test string foo\n',
               'blah.txt': '日本語',
               'frames/0/otherblah.f32.ind': np.random.rand(5),
               'frames/0/anotherblah.f64.ind': np.random.rand(7),
               'frames/0/number.i32.uni': 14}

    convertedRecords = {'frames/0/otherblah.f32.ind':
                        np.array(records['frames/0/otherblah.f32.ind'],
                                 dtype=np.float32)}

    success = True
    written = []
    with gtar.GTAR('test' + suffix, 'w') as arch:
        for path in records:
            arch.writePath(path, records[path])
            written.append(path)

            for readpath in sorted(written):
                equal = np.all(
                    arch.readPath(readpath) == convertedRecords.get(readpath, records[readpath]))
                if not equal:
                    print('Failed on readAndWrite() with {}: {}'.format(suffix, readpath))
                success = success and equal

    assert success
    return success

def main():
    success = True

    for suffix in ['.zip', '.tar', '.sqlite', '/']:
        try:
            success = test_readThenWrite(suffix) and success
            success = test_readAndWrite(suffix) and success
        except AssertionError:
            success = False

    sys.exit(not success)

if __name__ == '__main__': main()
