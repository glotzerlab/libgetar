project(getar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

FIND_PACKAGE (HDF5)
INCLUDE_DIRECTORIES (${HDF5_INCLUDE_DIR})
set (LINK_LIBS ${LINK_LIBS} hdf5 hdf5_cpp)

IF(ENABLE_HDF5)
  set(EXTRA_SRCS ${EXTRA_SRCS} src/Hdf5Archive.cpp)
  add_definitions(-DENABLE_HDF5)
ENDIF(ENABLE_HDF5)

set(GETAR_SRC
  src/Archive.cpp src/GTAR.cpp src/vogl_miniz_zip.cpp src/vogl_miniz.cpp
  src/Record.cpp src/TarArchive.cpp src/ZipArchive.cpp ${EXTRA_SRCS})

if(SHARED)
  ADD_LIBRARY(getar SHARED ${GETAR_SRC})
else()
  ADD_LIBRARY(getar ${GETAR_SRC})
endif()

target_link_libraries(getar ${LINK_LIBS})

add_subdirectory(test)