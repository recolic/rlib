# Compile and run tests for rlib
# CopyRight (C) 2017-2018 Recolic Keghart <root@recolic.net>
#
# Tests may fail on both compile-time(traits and meta-lib) and run-time(return non-zero) error.
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

# Test dependency:
# string ---> common 

MODULES=string meta trait stdio sio scope_guard

XTRA_FLAGS ?=
STD ?= 14
CXXFLAGS=-I. -I../.. $(XTRA_FLAGS) -std=c++$(STD)

POSTFIX=$(STD)_$(CXX)

all: string common header-include-all

common:
	$(CXX) $(CXXFLAGS) src/common.cc $(CXXFLAGS) -o src/common_$(POSTFIX).out
	src/common_$(POSTFIX).out

string:
	$(CXX) $(CXXFLAGS) src/string.cc $(CXXFLAGS) -o src/string_$(POSTFIX).out
	src/string_$(POSTFIX).out

header-include-all:
	$(CXX) $(CXXFLAGS) src/header-include-all.cc $(CXXFLAGS) -o src/header-include-all_$(POSTFIX).out
	src/header-include-all_$(POSTFIX).out

clean:
	rm -f src/*.out


