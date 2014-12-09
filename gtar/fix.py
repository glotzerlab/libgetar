
import argparse
import os
import re
import subprocess

from . import copy
from . import isZip64

parser = argparse.ArgumentParser(
    description='Command-line zip archive fixer')
parser.add_argument('input',
                    help='Input zip file to read')
parser.add_argument('-o', '--output', default='fixed.zip',
                    help='Output location for fixed zip archive')

def main(input, output):
    nameHalves = os.path.splitext(output)
    tempName = output

    if not os.path.exists(input):
        raise RuntimeError('Can\'t find input file {}'.format(input))

    while os.path.exists(tempName):
        nameHalves = (nameHalves[0] + '_', nameHalves[1])
        tempName = nameHalves[0] + nameHalves[1]

    cmdLine = ['zip', '-FF', input, '--out', tempName]

    proc = subprocess.Popen(cmdLine, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    (zipLog, _) = proc.communicate('y\n'.encode('UTF-8'))
    zipLog = zipLog.decode('utf8').split('\n')
    zipLog.pop()

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

    # if 'zip -FF' gave us a 32bit zip file, copy it into a 64bit archive.
    if not isZip64(tempName):
        print('Copying from zip32 to zip64...')
        copy.main(tempName, tempName)

    os.rename(tempName, output)

if __name__ == '__main__': main(**vars(parser.parse_args()))
