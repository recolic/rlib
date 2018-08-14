set(rlib_INCLUDE_DIRS ${PREFIX}/include)
set(rlib_LIBRARIES ${PREFIX}/lib/libr.a)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    # using clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(rlib_CXX_FLAGS "-Wno-terminate")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
    # using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    # using Visual Studio C++
endif()
