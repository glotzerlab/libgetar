CPP=c++

libgetar.so: Archive.cpp  Archive.hpp  GTAR.cpp  GTAR.hpp  miniz.c  SharedArray.hpp
	${CPP} Archive.cpp miniz.c -fPIC -shared -o libgetar.so

all: libgetar.so

clean:
	rm -f libgetar.so
