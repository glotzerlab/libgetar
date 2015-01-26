#!/usr/bin/env python

import os, subprocess, sys
from distutils.command.build_ext import build_ext
from distutils.core import Extension, setup
import numpy

macros = []
extra_args = []

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

if '--hdf5' in sys.argv:
    sys.argv.remove('--hdf5')
    macros.append(('ENABLE_HDF5', True))
    extra_args = subprocess.check_output(['h5c++', '-shlib', '-show'])
    extra_args = extra_args.decode('utf-8').strip().split()[1:]

if '--cython' in sys.argv:
    from Cython.Build import cythonize
    sys.argv.remove('--cython')

    def myCythonize(macros, *args, **kwargs):
        result = cythonize(*args, **kwargs)
        for r in result:
            r.define_macros.extend(macros)
            r.include_dirs.append(numpy.get_include())
            r.extra_compile_args.extend(extra_args)
            r.extra_link_args.extend(extra_args)

        return result

    modules = myCythonize(macros, 'gtar/_gtar.pyx')
else:
    sources = ['gtar/_gtar.cpp', 'src/Archive.cpp', 'src/vogl_miniz.cpp',
               'src/TarArchive.cpp', 'src/ZipArchive.cpp', 'src/Hdf5Archive.cpp',
               'src/vogl_miniz_zip.cpp', 'src/GTAR.cpp', 'src/Record.cpp']
    modules = [Extension('gtar._gtar', sources=sources,
                         define_macros=macros, extra_compile_args=extra_args,
                         extra_link_args=extra_args, include_dirs=[numpy.get_include()])]

setup(name='gtar',
      version='0.3.5',
      description='GTAR format file wrapper',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      url='',
      packages=['gtar'],
      ext_modules=modules
)
