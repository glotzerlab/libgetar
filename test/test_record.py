import random
import sys
import gtar

def randomPath(term=.25):
    """Generates a random (but valid) path name for a property given a
    chance to terminate after any segment"""
    validNameChars = ([chr(c) for c in range(ord('a'), ord('z')+1)] +
                      [chr(c) for c in range(ord('A'), ord('A')+1)] +
                      [chr(c) for c in range(ord('0'), ord('9')+1)] + ['.', '/'])
    meaningfulBits = ['frames', 'vars', '.f32', '.f64', '.u8', '.u64', '.i32',
                      '.i64', '.ind', '.uni']
    randomBits = [''.join(random.sample(validNameChars, random.randint(1, 10))) for _ in range(20)]
    allBits = meaningfulBits + randomBits

    result = ''

    while random.random() > term:
        seg = ''.join(random.sample(allBits, random.randint(1, 5)))
        result += seg + '/'

    # get rid of trailing slash
    result = result[:-1]

    return (result if result else randomPath(term=term))

def test_pathIdentity(n=100):
    """Creating a Record object from a reasonable path and asking for
    its path should always be identity"""
    success = True
    for _ in range(n):
        path = randomPath()
        rec = gtar.Record(path)
        reconstructedPath = rec.getPath()

        relPath = path.lstrip('/')

        if relPath != reconstructedPath:
            print('Error reconstructing path from Record!'
                  ' "{}" != "{}" [{}, {}, {}]'.format(
                      relPath, reconstructedPath, rec.getGroup(), rec.getName(), rec.getIndex()))
            success = False

    assert success
    return success

def test_pathKnown():
    success = True

    (ind, uni) = (gtar.Resolution.Individual, gtar.Resolution.Uniform)
    txt = gtar.Resolution.Text
    (f32, u8, u32) = (gtar.Format.Float32, gtar.Format.UInt8, gtar.Format.UInt32)

    recs = {
        ('', 'position', '1000', f32, ind): gtar.Record('frames/1000/position.f32.ind'),
        ('rigid', 'body', '', u32, ind): gtar.Record('rigid/body.u32.ind'),
        ('rigid', 'com', 'ab cd', f32, ind): gtar.Record('rigid/frames/ab cd/com.f32.ind'),
        ('', 'N', '', u32, uni): gtar.Record('N.u32.uni'),
        ('', 'test.txt', '', u8, txt): gtar.Record('test.txt'),
        ('hoomd', 'types.json', '1024', u8, txt): gtar.Record('hoomd/frames/1024/types.json')
        }

    for key in recs:
        (group, name, index, fmt, res) = key
        rec = recs[key]

        match = True
        match = match and rec.getGroup() == group
        match = match and rec.getName() == name
        match = match and rec.getIndex() == index
        match = match and rec.getFormat() == fmt
        match = match and rec.getResolution() == res

        if not match:
            print('Error reconstructing known path {} (reconstructed path '
                  '"{}")!'.format(key, rec.getPath()))

        success = success and match

    assert success
    return success

def main():
    success = True

    for test in [test_pathIdentity, test_pathKnown]:
        try:
            success = test() and success
        except AssertionError:
            success = False

    sys.exit(not success)

if __name__ == '__main__': main()
