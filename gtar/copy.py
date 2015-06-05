"""
Copy each record from one getar-formatted file to another.

::

   usage: python -m gtar.copy [-h] input output

   Command-line archive copier or translator

   positional arguments:
     input           Input file to read
     output          File to write to

   optional arguments:
     -h, --help      show this help message and exit
"""

import argparse

from . import cat

parser = argparse.ArgumentParser(
    description='Command-line archive copier or translator')
parser.add_argument('input',
                    help='Input file to read')
parser.add_argument('output',
                    help='File to write to')

def main(input, output):
    """Copy each record from one getar-formatted file to another.

    :param input: Input filename
    :param output: Output filename (can be the same as input)

    """
    cat.main(inputs=[input], output=output)

if __name__ == '__main__': main(**vars(parser.parse_args()))
