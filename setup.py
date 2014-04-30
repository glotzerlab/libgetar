#!/usr/bin/env python

import sys
from distutils.core import Extension, setup

macros = []

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

cFiles = ['Archive.cpp', 'capi.cpp', 'GTAR.cpp', 'Record.cpp', 'vogl_miniz.cpp', 'vogl_miniz_zip.cpp']
moduleSrc = ['src/{}'.format(name) for name in cFiles]
cExt = Extension('gtar._libgetar', moduleSrc, define_macros=macros)

setup(name='gtar',
      version='0.1',
      description='GTAR format file wrapper',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      url='',
      packages=['gtar'],
      package_dir={'gtar': 'pymodule'},
      ext_modules=[cExt]
)
