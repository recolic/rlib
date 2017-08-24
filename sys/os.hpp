#ifndef R_OS_HPP
#define R_OS_HPP

#ifndef __OS_ID__


#define __OS_ID__ UNKNOWN
#endif

#include "compiler_detector"
// Define __COMPILER_ID__ and __COMPILER_VER__

#ifdef __cplusplus

class OSInfo
{
public:
    enum class os_t {UNKNOWN, WINDOWS, LINUX, MACOS, BSD};
    enum class compiler_t {UNKNOWN, GCC, CLANG, MSVC, INTELC, BORLAND, IARC, SOLARIS, }; //Compiler which not supports cxx1x yet is not listed here. 201708.
#if defined(__OS_ID__)
    static constexpr os_t os = os_t::__OS_ID__;
#endif
#if defined(__COMPILER_ID__)
    static constexpr compiler_t compiler = compiler_t::__COMPILER_ID__;
#endif
#if defined(__COMPILER_VER__)
    static constexpr auto compiler_version = __COMPILER_VER__;
#else
    static constexpr auto compiler_version = 0;
#endif
};

#endif

#endif
