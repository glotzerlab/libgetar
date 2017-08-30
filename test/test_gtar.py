import functools
import unittest
import sys
import gtar
import numpy as np

SUFFIXES = ['.tar', '.zip', '.sqlite', '/']

SPECIAL_SUFFIX_NAMES = {'/': 'directory'}

def suffixed(f, suffix):
    """Decorator to instantiate a class method with a suffix"""

    def result(self):
        return f(self, suffix)

    result.__doc__ = f.__doc__
    return result

class MultiSuffixMeta(type):
    """Metaclass to instantiate tests for multiple file suffixes"""
    def __new__(cls, name, bases, dct):
        to_specialize = [key for key in dct if key.startswith('test_')]

        for fun_name in to_specialize:
            original_function = dct[fun_name]
            for suffix in SUFFIXES:
                name_suffix = SPECIAL_SUFFIX_NAMES.get(suffix, suffix[1:])
                new_name = '{}_{}'.format(fun_name, name_suffix)
                dct[new_name] = suffixed(original_function, suffix)
            del dct[fun_name]

        return super(MultiSuffixMeta, cls).__new__(cls, name, bases, dct)

class TestGTAR(unittest.TestCase):
    def test_readThenWrite(self, suffix):
        records = {'test.txt': 'test string foo\n',
                   'blah.txt': 'another string',
                   'frames/0/otherblah.f32.ind': np.random.rand(5),
                   'frames/0/anotherblah.f64.ind': np.random.rand(7),
                   'frames/0/number.i32.uni': 14}

        convertedRecords = {'frames/0/otherblah.f32.ind':
                            np.array(records['frames/0/otherblah.f32.ind'],
                                     dtype=np.float32)}

        with gtar.GTAR('test' + suffix, 'w') as arch:
            for path in records:
                arch.writePath(path, records[path])

        for mode in ['r', 'a']:
            with gtar.GTAR('test' + suffix, mode) as arch:
                for path in records:
                    self.assertTrue(np.all(
                        arch.readPath(path) == convertedRecords.get(path, records[path])))

    def test_readAndWrite(self, suffix):
        records = {'test.txt': 'test string foo\n',
                   'blah.txt': 'another string',
                   'frames/0/otherblah.f32.ind': np.random.rand(5),
                   'frames/0/anotherblah.f64.ind': np.random.rand(7),
                   'frames/0/number.i32.uni': 14}

        convertedRecords = {'frames/0/otherblah.f32.ind':
                            np.array(records['frames/0/otherblah.f32.ind'],
                                     dtype=np.float32)}

        written = []
        with gtar.GTAR('test' + suffix, 'w') as arch:
            for path in records:
                arch.writePath(path, records[path])
                written.append(path)

                for readpath in sorted(written):
                    self.assertTrue(np.all(
                        arch.readPath(readpath) == convertedRecords.get(readpath, records[readpath])))

    def test_readAndWriteRecords(self, suffix):
        with gtar.GTAR('test' + suffix, 'w') as arch:
            reclen = len(arch.getRecordTypes())
            for idx in range(10):
                arch.writeStr(str(idx), '{}_contents'.format(idx))

                self.assertEqual(reclen + 1, len(arch.getRecordTypes()))
                reclen = len(arch.getRecordTypes())

    def test_overwrite(self, suffix):
        with gtar.GTAR('test' + suffix, 'w') as arch:
            arch.writeStr('test.txt', 'bad')
            arch.writeStr('test.txt', 'good')

            self.assertEqual(arch.readStr('test.txt'), 'good')

        with gtar.GTAR('test' + suffix, 'r') as arch:
            self.assertEqual(arch.readStr('test.txt'), 'good')

    def test_read_closed(self, suffix):
        with gtar.GTAR('test' + suffix, 'w') as arch:
            arch.writeStr('test.txt', 'good')

        # throw when reading from a closed archive
        with self.assertRaises(RuntimeError):
            arch.readStr('test.txt')

        # readPath goes through getRecord
        with self.assertRaises(RuntimeError):
            arch.readPath('test.txt')

    def test_write_readonly(self, suffix):
        with gtar.GTAR('test' + suffix, 'w') as arch:
            arch.writeStr('test.txt', 'good')

        with gtar.GTAR('test' + suffix, 'r') as arch:
            with self.assertRaises(RuntimeError):
                arch.writeStr('test.txt', 'bad')

TestGTAR = MultiSuffixMeta(
    TestGTAR.__name__, TestGTAR.__bases__, dict(TestGTAR.__dict__))

if __name__ == '__main__':
    unittest.main()
