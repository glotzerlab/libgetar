## v1.1.3

- Support python 3.10 (mspells)

## v1.1.2

- Automatically open files in write mode, instead of append mode, if they have 0 size (mspells)

## v1.1.1

- Update minimum CMake version requirement (joaander)
- Update CI and wheel support for python 3.9 (mspells,bdice)

## v1.1.0

- Add group and group prefix selection to methods like `GTAR.getRecordTypes()` (mspells)

## v1.0.1

- Remove warnings from use of readdir_r (mspells)

## v1.0

- Binary wheel releases (bdice)
- Add SharedPtr wrapper for SharedArray (mspells)
- **API change**: GTAR::readUniform() now returns a SharedPtr instead of a std::auto_ptr (mspells)

## v0.7

- Use embedded versions of miniz, sqlite3, and dirent (bdice)
- Fix builds and tests on windows (bdice)

## v0.6

- Add usage of implicit index in GTAR.getRecord() (bdice)

## v0.5

- Add gtar.{cat,read} utilities (mspells)
- Preliminary java bindings (mspells)
- Experimental directory "archive" support (mspells)

## v0.4

- Add sqlite archive support (mspells)
- Add BulkWriter API (mspells)

## v0.3

- Add GTAR.{writeRecord,close}() (mspells)
- Add gtar.{copy,fix} utilities (mspells)

## v0.2

- GTAR.{read,write}Path() (mspells)

## v0.1

- zip, tar support (mspells)
