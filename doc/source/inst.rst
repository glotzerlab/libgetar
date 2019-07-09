============================
Installation and Basic Usage
============================

Installation
============

.. note::

   When building the `gtar` module (even when installing from PyPI), a
   working compiler chain is required. This means that, on Mac OSX
   systems, you will need to install the XCode command line tools if
   they are not already installed.

Versioned releases
------------------

Install a released version from PyPI using `pip`::

  pip install gtar

From source
-----------

Installing the libgetar python module (``gtar``) from source is
straightforward. From the root directory of the project:

::

   pip install .
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

The latest version of the documentation is available online on `ReadTheDocs
<http://libgetar.readthedocs.io/en/latest/>`_.
