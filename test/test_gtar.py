import os
import shutil
import sys
import unittest
import gtar
import numpy as np

def CaseFactory(suffix):
    class TestCases(unittest.TestCase):
        def __init__(self, methodName):
            super(TestCases, self).__init__(methodName)
            self.suffix = suffix

        def setUp(self):
            if os.path.exists('test' + self.suffix):
                if self.suffix == '/':
                    shutil.rmtree('test/')
                else:
                    os.remove('test' + self.suffix)

        def tearDown(self):
            if os.path.exists('test' + self.suffix):
                if self.suffix == '/':
                    shutil.rmtree('test/')
                else:
                    os.remove('test' + self.suffix)

        def test_readThenWrite(self):
            records = {'test.txt': 'test string foo\n',
                       'blah.txt': '日本語',
                       'frames/0/otherblah.f32.ind': np.random.rand(5),
                       'frames/0/anotherblah.f64.ind': np.random.rand(7),
                       'frames/0/number.i32.uni': 14}

            convertedRecords = {'frames/0/otherblah.f32.ind':
                                np.array(records['frames/0/otherblah.f32.ind'],
                                         dtype=np.float32)}

            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                for path in records:
                    arch.writePath(path, records[path])

            for mode in ['r', 'a']:
                with gtar.GTAR('test' + self.suffix, mode) as arch:
                    for path in records:
                        equal = np.all(
                            arch.readPath(path) == convertedRecords.get(path, records[path]))
                        if not equal:
                            print('Failed on readThenWrite() with {} ({} mode): {}'.format(
                                self.suffix, mode, path))
                        self.assertTrue(equal)

        def test_readAndWrite(self):
            records = {'test.txt': 'test string foo\n',
                       'blah.txt': '日本語',
                       'frames/0/otherblah.f32.ind': np.random.rand(5),
                       'frames/0/anotherblah.f64.ind': np.random.rand(7),
                       'frames/0/number.i32.uni': 14}

            convertedRecords = {'frames/0/otherblah.f32.ind':
                                np.array(records['frames/0/otherblah.f32.ind'],
                                         dtype=np.float32)}

            written = []
            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                for path in records:
                    arch.writePath(path, records[path])
                    written.append(path)

                    for readpath in sorted(written):
                        equal = np.all(
                            arch.readPath(readpath) == convertedRecords.get(readpath, records[readpath]))
                        if not equal:
                            print('Failed on readAndWrite() with {}: {}'.format(self.suffix, readpath))
                        self.assertTrue(equal)

        def test_readAndWriteRecords(self):
            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                reclen = len(arch.getRecordTypes())
                for idx in range(10):
                    arch.writeStr(str(idx), '{}_contents'.format(idx))

                    self.assertEqual(reclen + 1, len(arch.getRecordTypes()))
                    reclen = len(arch.getRecordTypes())

        def test_overwrite(self):
            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                arch.writeStr('test.txt', 'bad')
                arch.writeStr('test.txt', 'good')

                self.assertEqual(arch.readStr('test.txt'), 'good')

            with gtar.GTAR('test' + self.suffix, 'r') as arch:
                self.assertEqual(arch.readStr('test.txt'), 'good')

        def test_sql_records(self):
            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                arch.writePath('test.f32.uni', 14)

                for (rec,) in arch.querySql('SELECT record FROM records'):
                    self.assertEqual(rec, gtar.Record('test.f32.uni'))

                arch.writePath('test.f64.uni', 140)

                self.assertEqual(len(list(arch.querySql('SELECT record from records'))), 2)

        def test_sql_data(self):
            with gtar.GTAR('test' + self.suffix, 'w') as arch:
                arch.writePath('test.f32.uni', 14)

                for (data,) in arch.querySql('SELECT data FROM records'):
                    self.assertEqual(data, 14)

                arch.writePath('test.f64.uni', 140)

                for (data,) in arch.querySql('SELECT data FROM records WHERE format = ?', (gtar.Format.Float64,)):
                    self.assertEqual(data, 140)

        def __str__(self):
            return '{}({})'.format(TestCases.__name__, self.suffix)

    return TestCases

TestTar = CaseFactory('.tar')
TestZip = CaseFactory('.zip')
TestSqlite = CaseFactory('.sqlite')
TestDirectory = CaseFactory('/')

if __name__ == '__main__':
    unittest.main()
