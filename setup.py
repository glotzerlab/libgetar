#!/usr/bin/env python

import os, subprocess, sys
from distutils.command.build_ext import build_ext
from setuptools import Extension, setup
import numpy

long_description = """
libgetar is a library to read and write GEneric Trajectory ARchives, a
binary data format designed for efficient, extensible storage of
trajectory data.

For more details, consult the documentation at https://libgetar.readthedocs.io .
"""

with open('gtar/version.py') as version_file:
    exec(version_file.read())

macros = []
extra_args = []
sources = [
    'src/Archive.cpp',
    'src/DirArchive.cpp',
    'src/GTAR.cpp',
    'src/Record.cpp',
    'src/SqliteArchive.cpp',
    'src/TarArchive.cpp',
    'src/ZipArchive.cpp',
    'lz4/lz4.c',
    'lz4/lz4hc.c',
    'miniz/miniz.cpp',
    'sqlite3/sqlite3.c',
]

# prefix module to avoid collisions with builtin versions of libgetar
macros.append(('GTAR_NAMESPACE_PARENT', 'gtar_pymodule'))

if '--disable-read-check' in sys.argv:
    macros.append(('MINIZ_DISABLE_ZIP_READER_CRC32_CHECKS', None))
    sys.argv.remove('--disable-read-check')

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
      description='Read and write archive-based trajectory formats',
      author='Matthew Spellings',
      author_email='mspells@umich.edu',
      classifiers=[
          'Development Status :: 5 - Production/Stable',
          'License :: OSI Approved :: MIT License',
          'Programming Language :: Python :: 2',
          'Programming Language :: Python :: 3',
          'Topic :: System :: Archiving',
          'Topic :: Scientific/Engineering',
          'Topic :: Scientific/Engineering :: Chemistry',
          'Topic :: Scientific/Engineering :: Physics',
      ],
      ext_modules=modules,
      license='MIT',
      long_description=long_description,
      packages=['gtar'],
      project_urls={
          'Documentation': 'http://libgetar.readthedocs.io/',
          'Source': 'https://github.com/glotzerlab/libgetar'
          },
      install_requires=['numpy'],
      url='http://libgetar.readthedocs.io/',
)
