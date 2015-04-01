.. libgetar documentation master file, created by
   sphinx-quickstart on Tue Dec 16 09:20:17 2014.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

====================================
Welcome to libgetar's documentation!
====================================

Contents:

.. toctree::
   :maxdepth: 2

   gtar
   py/gtar
   cpp/gtar
   issues

============
Introduction
============

libgetar is a library to read and write GEneric Trajectory ARchives, a
binary data format designed for extensibility.

============
Installation
============

On nyx, you can just use the libgetar module.

Installing the libgetar python module (``gtar``) itself is
straightforward. From the root directory of the project:

::

   python setup.py install --user

=============
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

I also plan to keep a PDF version of the documentation available at
`the bitbucket download page
<https://bitbucket.org/glotzer/libgetar/downloads>`_.

==================
Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
