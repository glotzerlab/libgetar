
"""
Fix a getar-formatted zip file.

::

   usage: python -m gtar.fix [-h] [-o OUTPUT] input

   Command-line zip archive fixer

   positional arguments:
     input                 Input zip file to read

   optional arguments:
     -h, --help            show this help message and exit
     -o OUTPUT, --output OUTPUT
   Output location for fixed zip archive
"""

import argparse
import os
import re
import subprocess
import sys

from . import GTAR
from . import copy
from . import isZip64

parser = argparse.ArgumentParser(
    description='Command-line zip archive fixer')
parser.add_argument('input',
                    help='Input zip file to read')
parser.add_argument('-o', '--output', default='fixed.zip',
                    help='Output location for fixed zip archive')

def rebuildZip(input, tempName):
    cmdLine = ['zip', '-FF', input, '--out', tempName]

    proc = subprocess.Popen(cmdLine, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    (zipLog, _) = proc.communicate('y\n'.encode('UTF-8'))
    zipLog = zipLog.decode('utf8').split('\n')
    zipLog.pop()

    if proc.returncode not in [0, 3]:
        raise RuntimeError('Error {} running zip command'.format(proc.returncode))

    return zipLog

def deleteBadFrames(tempName, zipLog):
    zipLog = list(zipLog)
    deleteLines = []
    # Bad zip records from miniz always have this size [citation needed]
    if zipLog[-1].strip().endswith('(4294967295 bytes)'):
        foundFrame = re.search('frames/([0-9]+)/', zipLog[-1])

        if foundFrame:
            frame = foundFrame.group(0)
            while frame in zipLog[-1]:
                deleteLines.append(zipLog.pop())
        else:
            deleteLines.append(zipLog.pop())

    toDelete = []
    pattern = re.compile('^\\s*copying: (\\S*)\\s+\\(\\d+ bytes\\)$', flags=re.UNICODE)
    for line in deleteLines:
        res = pattern.match(line)
        if res:
            toDelete.append(res.group(1))

    if toDelete:
        print('Removing the following files: {}'.format(', '.join(toDelete)))
        cmdLine = ['zip', '-d', tempName] + toDelete
        subprocess.check_output(cmdLine)

    return toDelete

def main(input, output):
    """Fix a getar-formatted zip file.

    :param input: Input filename
    :param output: Output filename (can be the same as input)

    """

    if input.endswith('.tar') or input.endswith('.sqlite'):
        with gtar.GTAR(input, 'a'):
            pass
        return

    needsFix = True

    # if it ain't broke, don't fix it
    try:
        with GTAR(input, 'r'):
            needsFix = False
            if os.path.samefile(input, output) and isZip64(output):
                return 0
    except Exception:
        pass

    nameHalves = os.path.splitext(output)
    tempName = output

    if not os.path.exists(input):
        raise RuntimeError('Can\'t find input file {}'.format(input))

    while os.path.exists(tempName):
        nameHalves = (nameHalves[0] + '_', nameHalves[1])
        tempName = nameHalves[0] + nameHalves[1]

    try:
        if needsFix:
            zipLog = rebuildZip(input, tempName)
            deleteBadFrames(tempName, zipLog)

        # if 'zip -FF' gave us a 32bit zip file, copy it into a 64bit archive.
        fixed = tempName if needsFix else input
        if not isZip64(fixed):
            print('Copying from zip32 to zip64...')
            copy.main(fixed, tempName)

        if os.path.exists(tempName) and not os.path.samefile(tempName, output):
            os.rename(tempName, output)
    finally:
        if os.path.exists(tempName) and not os.path.samefile(tempName, output):
            os.remove(tempName)

if __name__ == '__main__': main(**vars(parser.parse_args()))
