===========================
Supported libgetar Backends
===========================

Zip
===

The zip backend uses zip64 archives (see :ref:`Zip-vs-Zip64`) to store
data, optionally compressed using the deflate algorithm. The zip
format consists of a series of "local headers" followed by content,
with a central directory at the very end of the file which lists the
locations of all files present in the archive to allow for efficient
random acces. This makes it possible for forcefully-killed processes
to leave zip files without a central index; see
:ref:`Zip-Central-Directories`.

Performance-wise, the zip format reads, writes, and opens files at a
not-unbearably-slow rate. Its main drawback is the reliance on the
presence of the central directory.

Tar
===

The tar backend stores data in the standard tar format, currently with
no option of compression. The tar format stores a file header just
before the data of each file, but with no global index in the standard
format. Libgetar builds a global index upon opening a tar file, which
consists of scanning through the entire archive file by file. Tar
files should be robust to process death; in the worst case, only part
of the data of a file is written.

The tar format involves the least overhead of any libgetar backend, so
it is fast to read and write. However, building the index quickly
becomes time-consuming for large archives with many files stored
inside, causing file opens to be slow.

Sqlite
======

The sqlite backend stores data in an sqlite database. Currently, each
write is implemented as a transaction, which causes the write speed to
be low for large numbers of records (see `the sqlite faq
<https://www.sqlite.org/faq.html#q19>`_). Data are stored uncompressed
or compressed with `LZ4 and LZ4HC <https://github.com/Cyan4973/lz4>`_.
Unfortunately, storing data in sqlite breaks the ability to use common
archive tools to inspect and manipulate stored data, so these are less
portable outside of libgetar. Because transactions are atomic, sqlite
databases are robust to process death.

The sqlite backend should be expected to have moderately fast open
speeds, slow write speeds (for large numbers of independent writes;
use a C++ :cpp:class:`BulkWriter` object to write multiple records
within a single transaction), and fast read speeds.

Directory
=========

The **experimental** directory backend stores data directly on the
filesystem. Currently, data are only stored uncompressed. Because each
file access occurs in the filesystem, this backend is extremely robust
to process death.

Backend Summary
===============

In summary:

- Zip

  - Pros

    - Reasonably fast at everything
    - "Good" compression ratio

  - Cons

    - Weak to process death

- Tar

  - Pros

    - Fast reads and writes
    - Resilient

  - Cons

    - Slow to open with many files in an archive
    - No compression

- Sqlite

  - Pros

    - Fast for reading and opening
    - Resilient
    - Fast but less-powerful compression (LZ4)

  - Cons

    - No standard archive-type tools
    - Slow for many individual writes (use :cpp:class:`BulkWriter` for bulk writes)

- Directory

  - Pros

    - Native writing speed
    - Extremely resilient

  - Cons

    - No compression
    - Could stress filesystem with many entries
