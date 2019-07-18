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
