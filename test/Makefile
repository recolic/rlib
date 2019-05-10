# Compile and run tests for rlib
# CopyRight (C) 2017-2018 Recolic Keghart <root@recolic.net>
#
# Tests may fail on both compile-time(traits and meta-lib) and run-time(return non-zero).
# Use `make <module>` to build and run a module, 
# 	and `make` to build and run all modules.
# 
# ready-to-use modules:
# 	string      rlib/string.hpp
# 	meta        rlib/meta.hpp
# 	trait       rlib/traits.hpp
# 	stdio       rlib/stdio.hpp
# 	scope_guard rlib/scope_guard.hpp rlib/scope_guard_buffer.hpp
# 	opt         rlib/opt.hpp
# 	sio         rlib/sys/sio.hpp
# 	os          rlib/sys/os.hpp
# 	require     rlib/require/*.hpp

MODULES=string meta trait stdio sio scope_guard

CXXFLAGS=-I.
FLAGS14=-std=c++14 rlib/libr.a
FLAGS17=-std=c++17

all: cxx14_all cxx17_all

cxx14_all: string_14

cxx17_all: string_17

string_14:
	$(CXX) $(CXXFLAGS) src/string.cc $(FLAGS14) -o src/string_14.out
	src/string_14.out

string_17:
	$(CXX) $(CXXFLAGS) src/string.cc $(FLAGS17) -o src/string_17.out
	src/string_17.out




