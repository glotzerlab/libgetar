
import argparse
import os

import gtar

parser = argparse.ArgumentParser(
    description='Command-line archive copier or translator')
parser.add_argument('input',
                    help='Input file to read')
parser.add_argument('output',
                    help='File to write to')

def main(input, output):
    tempName = output

    while os.path.exists(tempName):
        tempName += '_'

    with gtar.GTAR(input, 'r') as inpFile, gtar.GTAR(tempName, 'w') as outFile:

        recs = {rec: inpFile.queryFrames(rec) for rec in inpFile.getRecordTypes()}

        for rec in recs:
            frames = recs[rec]

            for frame in frames:
                rec.setIndex(frame)
                outFile.writeRecord(rec, inpFile.getRecord(rec, frame))

    os.rename(tempName, output)

if __name__ == '__main__': main(**vars(parser.parse_args()))
