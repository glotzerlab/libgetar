project(getar)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

IF(NOT DISABLE_SQLITE)
  FIND_PACKAGE(SQLite3)
ENDIF(NOT DISABLE_SQLITE)

IF(SQLITE3_FOUND)
  INCLUDE_DIRECTORIES(${SQLITE3_INCLUDE_DIRS})
  SET(LINK_LIBS ${LINK_LIBS} ${SQLITE3_LIBRARIES})
  set(EXTRA_SRCS ${EXTRA_SRCS} src/SqliteArchive.cpp lz4/lz4.c lz4/lz4hc.c)
  add_definitions(-DENABLE_SQLITE)
ENDIF(SQLITE3_FOUND)

set(GETAR_SRC
  src/Archive.cpp src/GTAR.cpp vogl_miniz/vogl_miniz_zip.cpp vogl_miniz/vogl_miniz.cpp
  src/Record.cpp src/TarArchive.cpp src/ZipArchive.cpp src/DirArchive.cpp ${EXTRA_SRCS})

if(LIBGETAR_SHARED)
  ADD_LIBRARY(getar SHARED ${GETAR_SRC})
else()
  ADD_LIBRARY(getar STATIC ${GETAR_SRC})
endif()

target_link_libraries(getar ${LINK_LIBS})

install(TARGETS getar LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

add_subdirectory(test)
