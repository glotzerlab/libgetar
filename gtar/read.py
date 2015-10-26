"""
Create an interactive python shell with the given files opened for
reading.

::

   usage: read.py [-h] ...

   Interactive getar-format archive shell

   positional arguments:
     inputs      Input files to open

   optional arguments:
     -h, --help  show this help message and exit
"""

import argparse
import code
import os

import gtar

parser = argparse.ArgumentParser(
    description='Interactive getar-format archive shell')
parser.add_argument('inputs', nargs=argparse.REMAINDER,
                    help='Input files to open')

messageTemplate = """
This is a python interactive shell.

{}
"""

definitions = {
    'archives': 'a list of all input files, opened for reading'
    }

def main(inputs):
    archives = [gtar.GTAR(path, 'r') for path in inputs]

    if len(inputs) == 1:
        traj = archives[0]
        definitions['traj'] = 'the opened input file'
        records = {rec.getName(): rec for rec in traj.getRecordTypes()}
        definitions['records'] = 'a dictionary of available records, indexed by property name'
        recordFrames = {name: traj.queryFrames(records[name]) for name in records}
        definitions['recordFrames'] = 'a dictionary of available frames for each property by name'

    message = messageTemplate.format(
        '\n'.join('- {} is {}.'.format(k, definitions[k]) for k in sorted(definitions)))

    code.interact(banner=message, local=dict(globals(), **locals()))

if __name__ == '__main__': main(**vars(parser.parse_args()))
