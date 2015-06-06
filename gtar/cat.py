"""
Take records from multiple getar-formatted files and place them
into an output file. In case of name conflicts, records from the last
input file take precedence.

::

   usage: cat.py [-h] [-o OUTPUT] ...

   Command-line archive concatenation

   positional arguments:
     inputs                Input files to read

   optional arguments:
     -h, --help            show this help message and exit
     -o OUTPUT, --output OUTPUT
                           File to write to
"""

import argparse
from collections import defaultdict
import os

import gtar

parser = argparse.ArgumentParser(
    description='Command-line archive concatenation')
parser.add_argument('inputs', nargs=argparse.REMAINDER,
                    help='Input files to read')
parser.add_argument('-o', '--output', required=True,
                    help='File to write to')

def main(inputs, output):
    """Take all records from a set of getar-formatted files and output them to another.

    :param inputs: Input filenames to concatenate
    :param output: Output filename (can be the same as input)

    """
    nameHalves = os.path.splitext(output)
    tempName = output

    while os.path.exists(tempName):
        nameHalves = (nameHalves[0] + '_', nameHalves[1])
        tempName = nameHalves[0] + nameHalves[1]

    recordFiles = {}
    for input in inputs:
        with gtar.GTAR(input, 'r') as inpFile:
            recs = {rec: inpFile.queryFrames(rec) for rec in inpFile.getRecordTypes()}

            for rec in recs:
                frames = recs[rec]
                for frame in frames:
                    rec.setIndex(frame)
                    recordFiles[rec.getPath()] = input

    sourceRecords = defaultdict(list)
    for path in recordFiles:
        sourceRecords[recordFiles[path]].append(path)

    try:
        with gtar.GTAR(tempName, 'w') as out:
            for source in sourceRecords:
                allPaths = sorted(sourceRecords[source])
                with gtar.GTAR(source, 'r') as inp:
                    for path in allPaths:
                        out.writePath(path, inp.readPath(path))

        if not os.path.samefile(tempName, output):
            os.rename(tempName, output)
    finally:
        if os.path.exists(tempName) and not os.path.samefile(tempName, output):
            os.remove(tempName)

if __name__ == '__main__': main(**vars(parser.parse_args()))
