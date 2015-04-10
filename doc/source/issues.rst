============================
Known Issues and Solutions
============================

.. _Zip-Central-Directories:

Zip Central Directories
=======================

The zip file format stores a "table of contents" known as a central
directory at the end of the file. This allows zip archives to be
"random-access" in the sense that you don't have to visit every file
in the archive to know what files exist in the archive, but if a
process is terminated forcefully (kill -9 or hitting a wall clock
limit), libgetar will not get an opportunity to write the central
directory. In this case, the zip file will be unreadable until you
rebuild the central directory using the command line tool ``zip
-FF`` or the python module :py:mod:`gtar.fix` (which uses ``zip -FF``
and deletes all data from any frames that were removed in the
process). Example:

::

   python -m gtar.fix broken.zip -o fixed.zip

Some very large (>8GB) zip files seem to be unable to be fixed, even
with ``zip -FF``. In this case, to recover your data you can extract
it all using the ``jar`` tool, which does not even look at the central
directory when extracting:

::

   mkdir temp && cd temp
   jar xvf ../broken.zip
   zip -mr fixed.zip -xi ./*

.. _Zip-vs-Zip64:

Zip vs Zip64
============

The zip archives libgetar writes are always in the zip64 format. It
can read "normal" zip archives just fine, but appending to them will
not work since converting an archive in-place is unsafe in case of
errors. Running the :py:mod:`gtar.fix` or
:py:func:`gtar.copy.main` python modules will always convert a file to
zip64 format. Example:

::

   python -m gtar.copy 32bit.zip 64bit.zip

basic_string::_S_construct null not valid
=========================================

This is due to passing in a python string object instead of a bytes
object and is probably an error on my part. These errors look like
this:

::

   terminate called after throwing an instance of 'std::logic_error'
     what():  basic_string::_S_construct null not valid

If you see any of those, let me know!
