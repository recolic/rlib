#ifndef R_OS_HPP
#define R_OS_HPP

#ifndef RLIB_OS_ID
#if defined(_Windows) || defined(__WIN32__) || defined(_WIN64) || defined(WIN32)
#   define RLIB_OS_ID WINDOWS
#elif defined(__linux__) || defined(__linux)
#   define RLIB_OS_ID LINUX
#elif defined(__APPLE__)
#   include "TargetConditionals.h"
#   if TARGET_IPHONE_SIMULATOR
#   define RLIB_OS_ID IOS
#   elif TARGET_OS_IPHONE
#   define RLIB_OS_ID IOS
#   elif TARGET_OS_MAC
#   define RLIB_OS_ID MACOS
#   else
#   define RLIB_OS_ID UNKNOWN_UNIX
#   endif
#elif defined(__ANDROID__)
#   define RLIB_OS_ID ANDROID
#elif defined(__unix__) || defined(__unix)
#   define RLIB_OS_ID UNKNOWN_UNIX
#else
#   define RLIB_OS_ID UNKNOWN
#endif
#endif

#include "compiler_detector"
// Define RLIB_COMPILER_ID and RLIB_COMPILER_VER

#if __cplusplus >= 201103L
namespace rlib {
    class OSInfo
    {
    public:
        enum class os_t {UNKNOWN, WINDOWS, LINUX, MACOS, BSD, IOS, ANDROID, UNKNOWN_UNIX};
        enum class compiler_t {UNKNOWN, GCC, CLANG, MSVC, INTELC, BORLAND, IARC, SOLARIS, ZAPCC}; //Compiler which not supports cxx1x yet is not listed here. 201708.
        static constexpr os_t os = 
    #if defined(RLIB_OS_ID)
    os_t::RLIB_OS_ID;
    #else
    os_t::UNKNOWN;
    #endif
        static constexpr compiler_t compiler = 
    #if defined(RLIB_COMPILER_ID)
    compiler_t::RLIB_COMPILER_ID;
    #else
    compiler_t::UNKNOWN;
    #endif
        static constexpr auto compiler_version = 
    #if defined(RLIB_COMPILER_VER)
    RLIB_COMPILER_VER;
    #else
    0;
    #endif
    };
}

#endif

#endif
