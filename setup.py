#!/usr/bin/env python

from distutils.core import Extension, setup

cFiles = ['Archive.cpp', 'miniz.c', 'capi.cpp', 'GTAR.cpp']
moduleSrc = ['src/{}'.format(name) for name in cFiles]
cExt = Extension('gtar._libgetar', moduleSrc)

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
