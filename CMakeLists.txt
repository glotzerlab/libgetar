project(getar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

ADD_LIBRARY(getar
  src/Archive.cpp src/GTAR.cpp src/vogl_miniz_zip.cpp src/vogl_miniz.cpp
  src/Record.cpp src/TarArchive.cpp src/ZipArchive.cpp)

add_subdirectory(test)