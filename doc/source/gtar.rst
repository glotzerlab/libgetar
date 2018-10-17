==================================================
The GETAR file format: GEneric Trajectory ARchives
==================================================

The GETAR Format
================

The main idea behind GETAR (GEneric Trajectory ARchives;
pronounced like the instrument!) files is to simply use standard data
archival tools and embed a small amount of metadata into the filename
within the archive.  To efficiently support large trajectories, the
underlying archive format (.tar.gz, .zip, .7z...) would ideally support
efficient random access to files and should be relatively common so
that generic tools would also be useful. Each backend format has
different performance and stability characteristics; these are
discussed in more detail below.

GETAR Archives
==============

GETAR files are just normal archive files; a list of supported
backends is in the :doc:`backends section <backends>`.  Data are
stored in individual **records**, which are simply files in the
archive with a particular naming scheme.

Records
=======

GETAR records consist of three primary pieces of information:

- A **name**, which indicates what attributes the data are intended to represent
- A **resolution**, which indicates at what level of detail the data are stored
- A **behavior**, which indicates how the data are stored over time

Record Names
************

Record names have no restriction or meaning beyond the overlying
schema (properties named "position" indicate the position of
particles, etc.).

Record Resolutions
******************

There are three values that resolutions can have, two for binary data
and one for text-based data:

- A **uniform** resolution indicates that the data are binary and that
  they represent the entire system.
- An **individual** resolution indicates that the data are binary and
  that they represent individual particles, rigid bodies, types...
- A **text** resolution indicates that the data are stored in
  plaintext and represent the entire system.


Record Behaviors
****************

Record behaviors indicate how data are stored over time.
There are three defined behaviors for different use cases:

- **Discretely varying** - Data are associated with particular times;
  for example, particle positions in a MD simulation
- **Continuously varying** - Data are generated "asynchronously" with
  the simulation; for example, the text printed to standard output
  during a HOOMD simulation
- **Constant** - data are stored only once and do not vary over the
  simulation

Archive Storage Paths
=====================

Put briefly, the record behavior indicates the directory within the
zip archive where the data are stored and the record name and
resolution are encoded in the filename.  In the following paths,
``{name}`` will correspond to the record name, ``{suffix}`` will encode the
storage mode of the data (determined by the resolution and binary type
of the data), and ``{index}`` is a string which will be discussed further
as needed.

Record filename suffixes are generated based on the resolution and
binary type of the data stored.  They are of the form ``{type}.{res}``,
where res is "uni" for uniform properties, "ind" for individual
properties, and anything else for text properties.  The type field
indicates the binary storage mode of the data and is of the form
``{chartype}{bitsize}``.  Valid values of ``{chartype}`` are "i" for signed
integers, "u" for unsigned integers, and "f" for floating point data.
The ``{bitsize}`` field is the size of each element in the array of data
in bits.  For example, particle positions stored as 32-bit floating
point numbers would be named ``position.f32.ind``.  Arbitrary blobs of
binary data could be stored as bytestrings as ``blob.u8.uni`` while a
JSON-encoded parameters list could be stored simply as ``params.json``.

Discretely varying data are stored in ``frames/{index}/{name}.{suffix}``,
where the index is some meaningful string corresponding to the time of
the data.  Continuously varying data are stored in
``vars/{name}.{suffix}/{index}``, where the index is the string
representation of a natural number.  Continuously varying quantities
must have indices which are sequentially ordered beginning at 0 and
are intended to be concatenated for use by readers.  Constant
quantities are stored in ``{name}.{suffix}`` .

Additionally, a prefix can be prepended to paths to differentiate
records.  For example, it could be desirable to store the moment of
inertia of both individual particles as well as rigid bodies within a
system.  In this case, particle moments could be stored in
``moment_inertia.f32.ind``, while rigid body moments could be stored in
``rigid_body/moment_inertia.f32.ind`` .
