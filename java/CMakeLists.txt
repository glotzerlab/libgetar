CMAKE_MINIMUM_REQUIRED(VERSION 2.6.2 FATAL_ERROR)

find_package(Java)
include(UseJava)
find_package(JNI)

set(LIBGETAR_SHARED ON)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/libgetar)
get_directory_property(GETAR_SRC_ DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/.. DEFINITION GETAR_SRC)

foreach(v ${GETAR_SRC_})
  list(APPEND GETAR_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../${v})
endforeach(v)

MESSAGE( STATUS "Java dirs: " ${JNI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS} ../src)

ADD_LIBRARY(libgetar_java_wrap SHARED ${GETAR_SRC} libgetar_java_wrap.cpp)

FILE(GLOB JAVA_SRCS . *.java)

add_jar(libgetar_wrap SOURCES ${JAVA_SRCS})
install_jar(libgetar_wrap ${CMAKE_INSTALL_PREFIX})
install(TARGETS libgetar_java_wrap DESTINATION ${CMAKE_INSTALL_PREFIX})

add_custom_target(swig_bindings
  COMMAND swig -c++ -java -package libgetar_wrap -o libgetar_java_wrap.cpp libgetar.i
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})