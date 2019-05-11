CXX ?= g++
CC ?= gcc
AR ?= ar
CXXFLAGS = -O3 -std=c++1z -fPIC
CFLAGS = 
ARFLAGS = rcs

PREFIX ?= /usr

def: compile_library

compile_library:
	$(CXX) $(CXXFLAGS) -c libr.cc -I . -o libr.o
	$(AR) $(ARFLAGS) libr.a libr.o

install_header:
	[ ! -d $(PREFIX)/include/rlib ] || rm -rf $(PREFIX)/include/rlib
	cp -r . $(PREFIX)/include/rlib
	rm -rf $(PREFIX)/include/rlib/test $(PREFIX)/include/rlib/.git

install_library: compile_library
	cp libr.a $(PREFIX)/lib/

install_cmake: install_library
	[ ! -d $(PREFIX)/lib/cmake/rlib ] || rm -rf $(PREFIX)/lib/cmake/rlib
	[ ! -d $(PREFIX)/lib/cmake ] || cp -r cmake $(PREFIX)/lib/cmake/rlib

install: install_header install_library install_cmake

uninstall:
	rm -rf $(PREFIX)/include/rlib $(PREFIX)/lib/cmake/rlib
	rm -f $(PREFIX)/lib/libr.a

clean:
	rm *.o *.a

.PHONY: test

test:
	cd test && ./test.sh

