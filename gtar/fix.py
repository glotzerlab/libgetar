
import argparse
import os
import re
import subprocess

from . import copy

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

    # Sometimes 'zip -FF' just directly copies improperly-closed
    # archives instead of actually adding the central directory. If
    # the file didn't change from running 'zip -FF', instead run
    # gtar.copy which will write a central directory.
    cmdLine = ['md5sum', input, tempName]
    lines = subprocess.check_output(cmdLine).decode('utf8').split('\n')
    checksums = [line.split()[0] for line in lines if line]

    if checksums[0] == checksums[1]:
        copy.main(tempName, tempName)

    if toDelete:
        print('Removing the following files: {}'.format(', '.join(toDelete)))
        cmdLine = ['zip', '-d', tempName] + toDelete
        subprocess.check_output(cmdLine)

    os.rename(tempName, output)

if __name__ == '__main__': main(**vars(parser.parse_args()))
