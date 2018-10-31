# Use this file if you want to include rlib as a subdirectory

add_library(r STATIC ../libr.cc)
include_directories(..)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(rlib_CXX_FLAGS "-Wno-terminate")
endif()

