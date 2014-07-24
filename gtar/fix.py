
import argparse
import os
import re
import subprocess

parser = argparse.ArgumentParser(
    description='Command-line archive fixer')
parser.add_argument('input',
                    help='Input file to read')
parser.add_argument('-o', '--output', default='fixed.zip',
                    help='Output location for fixed archive')

def main(input, output):
    tempName = output

    while os.path.exists(tempName):
        tempName += '_'

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

    os.rename(tempName, output)

if __name__ == '__main__': main(**vars(parser.parse_args()))
