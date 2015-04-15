import argparse
import json
import os
import sys
import timeit

import numpy as np

imports = """
import random
from subprocess import call, PIPE
import numpy as np
import gtar
"""

generateRandom = """
position = np.random.uniform(-50, 50, size=({Nframes:}, {N:}, 3)).astype(np.float32)
orientation = np.random.uniform(-1, 1, size=({Nframes:}, {N:}, 4)).astype(np.float32)
"""

testWrite = """
with gtar.GTAR('{filename:}', 'w') as traj, traj.getBulkWriter() as writer:
    for i, (pos, quat) in enumerate(zip(position, orientation)):
        writer.writePath('frames/{{}}/position.f32.ind'.format(i), pos)
        writer.writePath('frames/{{}}/orientation.f32.ind'.format(i), quat)
"""

clearCache = """
call(['sudo', '/bin/sync']);
call(['sudo', '/sbin/sysctl', 'vm.drop_caches=3'], stdout=PIPE);
"""

testOpen = """
with gtar.GTAR('{filename:}', 'r') as traj:
    pass
"""

testRead = """
val = 0
with gtar.GTAR('{filename:}', 'r') as traj:
    for (_, (pos, quat)) in traj.recordsNamed(['position', 'orientation']):
        pass
#        val += np.sum(pos) + np.sum(quat)
"""

testRandomRead = """
val = 0
with gtar.GTAR('{filename:}', 'r') as traj:
    ((posrec, quatrec), frames) = traj.framesWithRecordsNamed(['position', 'orientation'])
    random.shuffle(frames)

    for frame in frames:
        traj.getRecord(posrec, frame)
#        val += np.sum(traj.getRecord(posrec, frame))
    for frame in frames:
        traj.getRecord(quatrec, frame)
#        val += np.sum(traj.getRecord(quatrec, frame))
"""

parser = argparse.ArgumentParser(
    description='Run some GTAR speed benchmarks')
parser.add_argument('--number', type=eval, required=True,
                    help='List of numbers of particles to evaluate for')
parser.add_argument('--byte-counts', type=eval, required=True,
                    help='List of numbers of byte sizes to evaluate for')
parser.add_argument('-o', '--output', default='show',
                    help='Output file location')
parser.add_argument('--replicants', type=int, default=5,
                    help='Number of replicants to run')
parser.add_argument('--filenames', default='test_.zip,test_.tar,test_.sqlite',
                    help='File names to open')
parser.add_argument('--tests', default='Open,COpen,Write,Read,CRead,RRead,CRRead',
                    help='Tests to run')

def runTest(name, **kwargs):
    if name == 'Open':
        return openSpeed(**kwargs)[0]
    elif name == 'COpen':
        return openSpeed(withCache=True, **kwargs)[0]
    elif name == 'Write':
        (byteCount, time, _) = writeSpeed(**kwargs)
        return byteCount/time
    elif name == 'Read':
        (byteCount, time, _) = readSpeed(**kwargs)
        return byteCount/time
    elif name == 'CRead':
        (byteCount, time, _) = readSpeed(withCache=True, **kwargs)
        return byteCount/time
    elif name == 'RRead':
        (byteCount, time, _) = readSpeed(shuffle=True, **kwargs)
        return byteCount/time
    elif name == 'CRRead':
        (byteCount, time, _) = readSpeed(withCache=True, shuffle=True, **kwargs)
        return byteCount/time
    elif name == 'Seek':
        (_, time, recCount) = readSpeed(shuffle=True, **kwargs)
        return time/recCount
    elif name == 'CSeek':
        (_, time, recCount) = readSpeed(withCache=True, shuffle=True, **kwargs)
        return time/recCount
    else:
        raise RuntimeError('Unknown test {}'.format(name))

def writeSpeed(filename, Nbytes, Nparticles, Nrep=5, **kwargs):
    """Returns (num_bytes, speed(s), num_records) for a write benchmark"""
    Nframes = int(np.ceil(Nbytes/(Nparticles*(3 + 4)*4)))
    setup = imports + generateRandom.format(Nframes=Nframes, N=Nparticles)
    stmt = testWrite.format(filename=filename)

    realBytes = Nframes*Nparticles*(3 + 4)*4
    time = min(timeit.repeat(stmt=stmt, setup=setup, repeat=Nrep, number=1))

    return (realBytes, time, 2*Nframes)

def openSpeed(filename, Nbytes, Nparticles, Nrep=5, withCache=False, **kwargs):
    """Returns (speed(s), num_records) for a file open benchmark"""
    Nframes = int(np.ceil(Nbytes/(Nparticles*(3 + 4)*4)))
    setup = (imports + generateRandom.format(Nframes=Nframes, N=Nparticles) +
             testWrite.format(filename=filename) +
             (testOpen.format(filename=filename) if withCache else clearCache))
    stmt = testOpen.format(filename=filename)

    time = min(timeit.repeat(stmt=stmt, setup=setup, repeat=Nrep, number=1))

    return (time, 2*Nframes)

def readSpeed(filename, Nbytes, Nparticles, Nrep=5, shuffle=False, withCache=False, **kwargs):
    """Returns (num_bytes, speed(s), num_records) for a read benchmark"""
    readText = testRandomRead if shuffle else testRead
    Nframes = int(np.ceil(Nbytes/(Nparticles*(3 + 4)*4)))
    setup = (imports + generateRandom.format(Nframes=Nframes, N=Nparticles) +
             testWrite.format(filename=filename) +
             (readText.format(filename=filename) if withCache else clearCache))
    stmt = readText.format(filename=filename)

    realBytes = Nframes*Nparticles*(3 + 4)*4
    time = min(timeit.repeat(stmt=stmt, setup=setup, repeat=Nrep, number=1))

    return (realBytes, time, 2*Nframes)

def main(number, byte_counts, output, replicants, filenames, tests):
    filenames = filenames.split(',')
    tests = tests.split(',')
    data = {}
    try:
        for filename in filenames:
            data[filename] = []
            for N in number:
                data[filename].append([])
                for filesize in byte_counts:
                    print(filename, N, filesize, file=sys.stderr)
                    data[filename][-1].append(
                        [runTest(name, filename=filename, Nbytes=filesize, Nparticles=N) for name in tests])
                    print('  ' +
                          ''.join(len(data[filename][-1][-1])*[' {:>4.2e}']).
                          format(*data[filename][-1][-1]), file=sys.stderr)
    except KeyboardInterrupt:
        print('Exiting due to interrupt')
    finally:
        for filename in filenames:
            if os.path.exists(filename):
                os.unlink(filename)

    if output == 'show':
        header = (2*' ' + '    {:>7}' + '    {:>9}' + ''.join(len(tests)*['    {:>9}'])).format(
            'N', 'Size/B', *tests)
        print(header)
        template = (2*' ' + '    {:>7d}' + '    {:>5.3e}' + ''.join(len(tests)*['    {:>5.3e}']))
        for filename in data:
            print('{}:'.format(filename))
            for (i, N) in enumerate(number):
                for (j, filesize) in enumerate(byte_counts):
                    try:
                        row = template.format(N, filesize, *data[filename][i][j])
                        print(row)
                    except IndexError:
                        pass
                    except KeyError:
                        pass
    else:
        json.dump({'headers': ['N', 'Size'] + tests, 'files': data}, open(output, 'w'))

if __name__ == '__main__': main(**vars(parser.parse_args()))
