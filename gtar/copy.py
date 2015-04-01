"""
Copy each record from one getar-formatted file to another.

::

   usage: python -m gtar.copy [-h] [--ignore-empty] input output

   Command-line archive copier or translator

   positional arguments:
     input           Input file to read
     output          File to write to

   optional arguments:
     -h, --help      show this help message and exit
     --ignore-empty  Ignore empty files
"""

import argparse
import os

import gtar

parser = argparse.ArgumentParser(
    description='Command-line archive copier or translator')
parser.add_argument('input',
                    help='Input file to read')
parser.add_argument('output',
                    help='File to write to')
parser.add_argument('--ignore-empty', action='store_true', default=False,
                    help='Ignore empty files')

def main(input, output, ignore_empty=False):
    """Copy each record from one getar-formatted file to another.

    :param input: Input filename
    :param output: Output filename (can be the same as input)
    :param ignore_empty: if True, ignore empty records rather than copying them over

    """

    nameHalves = os.path.splitext(output)
    tempName = output

    while os.path.exists(tempName):
        nameHalves = (nameHalves[0] + '_', nameHalves[1])
        tempName = nameHalves[0] + nameHalves[1]

    try:
        with gtar.GTAR(input, 'r') as inpFile, gtar.GTAR(tempName, 'w') as outFile:

            recs = {rec: inpFile.queryFrames(rec) for rec in inpFile.getRecordTypes()}

            for rec in recs:
                frames = recs[rec]

                for frame in frames:
                    rec.setIndex(frame)

                    data = inpFile.getRecord(rec, frame)
                    try:
                        nonempty = len(data)
                    except TypeError:
                        nonempty = data

                    if nonempty or not ignore_empty:
                        outFile.writeRecord(rec, data)

        if not os.path.samefile(tempName, output):
            os.rename(tempName, output)
    finally:
        if os.path.exists(tempName) and not os.path.samefile(tempName, output):
            os.remove(tempName)

if __name__ == '__main__': main(**vars(parser.parse_args()))
