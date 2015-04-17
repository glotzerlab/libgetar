project(getar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

IF(NOT DISABLE_SQLITE)
  FIND_PACKAGE(SQLite3)
ENDIF(NOT DISABLE_SQLITE)

IF(SQLITE3_FOUND)
  INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
  SET(LINK_LIBS ${LINK_LIBS} ${SQLITE3_LIBRARIES})
  set(EXTRA_SRCS ${EXTRA_SRCS} src/SqliteArchive.cpp src/lz4/lz4.c src/lz4/lz4hc.c)
  add_definitions(-DENABLE_SQLITE)
ENDIF(SQLITE3_FOUND)

IF(ENABLE_HDF5)
  FIND_PACKAGE (HDF5)
ENDIF(ENABLE_HDF5)

IF(HDF5_FOUND)
  INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})
  set (LINK_LIBS ${LINK_LIBS} hdf5 hdf5_cpp)
  set(EXTRA_SRCS ${EXTRA_SRCS} src/Hdf5Archive.cpp)
  add_definitions(-DENABLE_HDF5)
ENDIF(HDF5_FOUND)

set(GETAR_SRC
  src/Archive.cpp src/GTAR.cpp src/vogl_miniz_zip.cpp src/vogl_miniz.cpp
  src/Record.cpp src/TarArchive.cpp src/ZipArchive.cpp ${EXTRA_SRCS})

if(LIBGETAR_SHARED)
  ADD_LIBRARY(getar SHARED ${GETAR_SRC})
else()
  ADD_LIBRARY(getar STATIC ${GETAR_SRC})
endif()

target_link_libraries(getar ${LINK_LIBS})

add_subdirectory(test)
