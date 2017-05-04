#!/usr/bin/env python

import os, subprocess, sys
from distutils.command.build_ext import build_ext
from distutils.core import Extension, setup
import numpy

with open('gtar/version.py') as version_file:
    exec(version_file.read())

macros = []
extra_args = []
sources = ['src/Archive.cpp', 'vogl_miniz/vogl_miniz.cpp',
        'src/TarArchive.cpp', 'src/ZipArchive.cpp', 'vogl_miniz/vogl_miniz_zip.cpp',
        'src/GTAR.cpp', 'src/Record.cpp', 'src/DirArchive.cpp']

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

on_rtd = os.environ.get('READTHEDOCS') == 'True'
if '--disable-sqlite' in sys.argv:
    sys.argv.remove('--disable-sqlite')
elif on_rtd:
    pass
else:
    macros.append(('ENABLE_SQLITE', True))
    sources.extend(['lz4/lz4.c', 'lz4/lz4hc.c', 'src/SqliteArchive.cpp'])
    extra_args.append('-lsqlite3')

if '--debug' in sys.argv:
    extra_args.extend(['-O0', '-g'])
    sys.argv.remove('--debug')

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
            r.sources.extend(sources)

        return result

    modules = myCythonize(macros, 'gtar/_gtar.pyx')
else:
    sources.append('gtar/_gtar.cpp')
    modules = [Extension('gtar._gtar', sources=sources,
                         define_macros=macros, extra_compile_args=extra_args,
                         extra_link_args=extra_args, include_dirs=[numpy.get_include()])]

setup(name='gtar',
      version=__version__,
      description='GTAR format file wrapper',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      url='',
      packages=['gtar'],
      ext_modules=modules
)
