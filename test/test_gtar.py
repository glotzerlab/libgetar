from __future__ import print_function

import time
import os
import math
import numpy
import random
import sys
from subprocess import call, PIPE
import gtar

def write_frame(file, frame, position, orientation, compress_mode):
    file.writeBytes('{0}/position'.format(frame), position.tostring(), compress_mode);
    file.writeBytes('{0}/orientation'.format(frame), orientation.tostring(), compress_mode);

def read_frame(file, frame):
    position = file.readBytes('{0}/position'.format(frame));
    orientation = file.readBytes('{0}/orientation'.format(frame));
    return (numpy.fromstring(position, dtype='float32'), numpy.fromstring(orientation, dtype='float32'))

def write_random_file(file, nframes, N, compress_mode):
    step = int(nframes / 20);
    if step == 0:
        step = 1;

    for i in range(0,nframes):
        if i % step == 0:
            print(i, "/", nframes, file=sys.stderr)

        position = numpy.random.random((N,3)).astype('float32');
        orientation = numpy.random.random((N,4)).astype('float32');
        write_frame(file, i, position, orientation, compress_mode);

def read_sequential_file(file, nframes):
    step = int(nframes / 20);
    if step == 0:
        step = 1;

    for i in range(0,nframes):
        if i % step == 0:
            print(i, "/", nframes, file=sys.stderr)

        position, orientation = read_frame(file, i);

def read_random_file(file, nframes):
    step = int(nframes / 20);
    if step == 0:
        step = 1;

    frames = list(range(0,nframes));
    random.shuffle(frames);

    for i,f in enumerate(frames):
        if i % step == 0:
            print(i, "/", nframes, file=sys.stderr)

        position, orientation = read_frame(file, i);

def compute_nframes(N, size):
    bytes_per_frame = (3+4)*4 * N;
    return int(math.ceil(size / bytes_per_frame));

def compute_actual_size(N, nframes):
    bytes_per_frame = (3+4)*4 * N;
    return nframes * bytes_per_frame;

## Run all benchmarks with the given options
def run_benchmarks(N, size):
    compression = False;
    timings = {}

    compress_mode = gtar.CompressMode.NoCompress
    if compression:
        compress_mode = gtar.CompressMode.FastCompress

    nframes = compute_nframes(N, size);
    actual_size = compute_actual_size(N, nframes);

    # first, write the file and time how long it takes
    print("Writing file: ", file=sys.stderr)
    start = time.time();
    file = gtar.GTAR('test.zip', mode='w')
    write_random_file(file, nframes, N, compress_mode);
    del file;
    end = time.time();

    timings['write'] = actual_size / 1024**2 / (end - start);

    call(['sudo', '/bin/sync']);
    call(['sudo', '/sbin/sysctl', 'vm.drop_caches=3'], stdout=PIPE);

    # time how long it takes to open the file
    print("Opening file... ", file=sys.stderr, end='')
    start = time.time();
    file = gtar.GTAR('test.zip', mode='r')
    end = time.time();
    print(end - start, "s", file=sys.stderr);

    timings['open_time'] = (end - start);

    # Read the file sequentially and measure the time taken
    print("Sequential read file:", file=sys.stderr)
    start = time.time();
    read_sequential_file(file, nframes);
    end = time.time();

    timings['seq_read'] = actual_size / 1024**2 / (end - start);
    timings['seq_read_time'] = (end - start) / nframes / 1e-3;

    # If the size is small, read the file again (cached)
    if size < 10*1024**3:
        print("Sequential read file: (cached)", file=sys.stderr)
        start = time.time();
        read_sequential_file(file, nframes);
        end = time.time();

        timings['seq_cache_read'] = actual_size / 1024**2 / (end - start);
    else:
        timings['seq_cache_read'] = 0;

    # Read the file sequentially and measure the time taken
    print("Random read file:", file=sys.stderr)
    start = time.time();
    file = gtar.GTAR('test.zip', mode='r')
    read_random_file(file, nframes);
    end = time.time();

    timings['random_read'] = actual_size / 1024**2 / (end - start);
    timings['random_read_time'] = (end - start) / nframes / 1e-3;

    del file;
    os.unlink('test.zip')
    return timings

def run_sweep(size, size_str):

    if size < 10*1024**3:
        result = run_benchmarks(32*32, size)

        print("| {0} | {1} | {2:.3g} | {3:.4g} | {4:.4g} | {5:.4g} | {6:.4g} | {7:.2g} |".format(size_str, "32^2", result['open_time'], result['write'], result['seq_read'], result['seq_cache_read'], result['random_read'], result['random_read_time']));
        sys.stdout.flush();

    result = run_benchmarks(100*100, size)

    print("| {0} | {1} | {2:.4g} | {3:.3g} | {4:.4g} | {5:.4g} | {6:.4g} | {7:.2g} |".format(size_str, "100^2", result['open_time'], result['write'], result['seq_read'], result['seq_cache_read'], result['random_read'], result['random_read_time']));
    sys.stdout.flush();

    result = run_benchmarks(1000*1000, size)

    print("| {0} | {1} | {2:.4g} | {3:.3g} | {4:.4g} | {5:.4g} | {6:.4g} | {7:.2g} |".format(size_str, "1000^2", result['open_time'], result['write'], result['seq_read'], result['seq_cache_read'], result['random_read'], result['random_read_time']));
    sys.stdout.flush();


print("""
| Size | N   | Open time (s) | Write (MB/s) | Seq read (MB/s) | Seq read cached (MB/s) | Random read (MB/s) | Random read time (ms) |
| :--- | :-- | :-----------   | :---------   | :-----------    | :-----------           | :-----------       | :-----------          |""");

run_sweep(100*1024**2, "100 MiB");
run_sweep(1*1024**3, "1 GiB");
run_sweep(128*1024**3, "128 GiB");
