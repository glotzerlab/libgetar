project(getar)
cmake_minimum_required(VERSION 2.6.2...3.13 FATAL_ERROR)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

set(GETAR_SRC
    src/Archive.cpp
    src/DirArchive.cpp
    src/GTAR.cpp
    src/Record.cpp
    src/SqliteArchive.cpp
    src/TarArchive.cpp
    src/ZipArchive.cpp
    lz4/lz4.c
    lz4/lz4hc.c
    miniz/miniz.cpp
    sqlite3/sqlite3.c
)

set(GETAR_HEADERS
    src/Archive.hpp
    src/DirArchive.hpp
    src/GTAR.hpp
    src/Record.hpp
    src/SharedArray.hpp
    src/SqliteArchive.hpp
    src/TarArchive.hpp
    src/ZipArchive.hpp
    miniz/miniz.h
    sqlite3/sqlite3.h
)

include_directories(
  lz4
  miniz
  sqlite3
  )

SET(LINK_LIBS ${LINK_LIBS} ${CMAKE_DL_LIBS})

find_package(Threads)

if(LIBGETAR_SHARED)
  ADD_LIBRARY(getar SHARED ${GETAR_SRC})
else()
  ADD_LIBRARY(getar STATIC ${GETAR_SRC})
endif()

target_link_libraries(getar ${LINK_LIBS} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS getar LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

install(FILES ${GETAR_HEADERS} DESTINATION include/getar)

add_subdirectory(test)
