============================
Installation and Basic Usage
============================

Installation
============

Installing the libgetar python module (``gtar``) is straightforward.
From the root directory of the project:

::

   python setup.py install --user
   # Test installation:
   cd
   python -c 'import gtar'

Note that trying to run scripts from the libgetar source directory
will not work!

Documentation
=============

Documentation is built using sphinx and doxygen. To build it, use the
Makefile in the doc subdirectory:

::

   cd doc
   make html


Note that we use the ``breathe`` python package to interface between
sphinx and doxygen. If it isn't installed (and isn't available in your
package manager), you can install it easily with pip:

::

   pip install breathe --user

The latest version of the documentation is available `online
<http://libgetar.readthedocs.io/>`_.
