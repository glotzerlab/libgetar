#!/usr/bin/env python

import os, sys
from distutils.core import Extension, setup
import numpy

macros = []

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

if '--cython' in sys.argv:
    from Cython.Build import cythonize
    sys.argv.remove('--cython')

    def myCythonize(macros, *args, **kwargs):
        result = cythonize(*args, **kwargs)
        for r in result:
            r.define_macros.extend(macros)
            r.include_dirs.append(numpy.get_include())

        return result

    modules = myCythonize(macros, 'gtar/_gtar.pyx')
else:
    sources = ['gtar/_gtar.cpp', 'src/Archive.cpp', 'src/vogl_miniz.cpp',
               'src/TarArchive.cpp', 'src/ZipArchive.cpp',
               'src/vogl_miniz_zip.cpp', 'src/GTAR.cpp', 'src/Record.cpp']
    modules = [Extension('gtar._gtar', sources=sources,
                         include_dirs=[numpy.get_include()])]

setup(name='gtar',
      version='0.3.4',
      description='GTAR format file wrapper',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      url='',
      packages=['gtar'],
      ext_modules=modules
)
