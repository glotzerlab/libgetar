============================
Installation and Basic Usage
============================

Installation
============

On nyx, you can just use the libgetar module.

Elsewhere, installing the libgetar python module (``gtar``) is
straightforward. From the root directory of the project:

::

   python setup.py install --user
   # test installation
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
<http://glotzerlab.engin.umich.edu/libgetar/>`_.
