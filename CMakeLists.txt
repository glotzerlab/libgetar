project(getar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

ADD_LIBRARY(getar SHARED
  src/Archive.cpp src/vogl_miniz_zip.cpp src/vogl_miniz.cpp src/capi.cpp)