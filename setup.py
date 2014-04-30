#!/usr/bin/env python

import os, sys
from distutils.core import Extension, setup
from Cython.Build import cythonize

macros = []

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

def myCythonize(*args, **kwargs):
    result = cythonize(*args, **kwargs)
    for r in result:
        r.define_macros.extend(macros)

    return result

setup(name='gtar',
      version='0.1',
      description='GTAR format file wrapper',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      url='',
      packages=['gtar'],
      ext_modules=myCythonize('gtar/_gtar.pyx')
)
