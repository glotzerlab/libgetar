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
This is a python interactive shell. The variable "archives" is a list
of all input files, opened for reading.

{}
"""

def main(inputs):
    archives = [gtar.GTAR(path, 'r') for path in inputs]

    if len(inputs) == 1:
        traj = archives[0]
        message = messageTemplate.format('The variable "traj" is the opened '
                                 'input file.')
    else:
        message = messageTemplate.format('')

    code.interact(banner=message, local=dict(globals(), **locals()))

if __name__ == '__main__': main(**vars(parser.parse_args()))
