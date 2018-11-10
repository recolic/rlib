# Use this file if you want to include rlib as a subdirectory

add_library(r STATIC ../libr.cc)
include_directories(..)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(rlib_CXX_FLAGS "-Wno-terminate")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()

if(MINGW)
    add_definitions(-DRLIB_MINGW_DISABLE_INLINE_TLS)
endif()


