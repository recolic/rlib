#ifndef R_FDSET_HPP
#define R_FDSET_HPP

#include <unistd.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>

#include <rlib/sys/os.hpp>
#if RLIB_OS_ID == OS_WINDOWS
using fd = HANDLE;
#else
using fd = int;
#endif

#if RLIB_COMPILER_ID == CC_GCC 
#include <ext/stdio_filebuf.h>
namespace rlib {
    template <fd posix_handle>
    inline std::istream & fd_to_istream() {
         __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in);
         return std::ifstream(&filebuf);
    }
    template <fd posix_handle>
    inline std::ostream & fd_to_ostream() {
         __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::out);
         return std::ofstream(&filebuf);
    }
    template <fd posix_handle>
    inline std::iostream & fd_to_iostream() {
         __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in || std::ios::out);
         return std::fstream(&filebuf);
    }
} // rlib
#elif RLIB_COMPILER_ID == CC_MSVC
namespace rlib {
    template <fd posix_handle>
    inline std::istream & fd_to_istream() {
        ifstream fs(::_fdopen(posix_handle, "r"));
        return fs;
    }
    template <fd posix_handle>
    inline std::ostream & fd_to_ostream() {
        ofstream fs(::_fdopen(posix_handle, "w"));
        return fs;
    }
    template <fd posix_handle>
    inline std::iostream & fd_to_iostream() {
        fstream fs(::_fdopen(posix_handle, "rw"));
        return fs;
    }
} // rlib
#else
namespace rlib {
    template <fd>
    constexpr inline std::istream & fd_to_istream() {
        throw std::invalid_argument("fd != 0 to istream is not implemented except gcc/msvc.");
    }
    template <fd>
    constexpr inline std::ostream & fd_to_ostream() {
        throw std::invalid_argument("fd != 1/2 to ostream is not implemented except gcc/msvc.");
    }
    template <fd>
    constexpr inline std::iostream & fd_to_iostream() {
        throw std::invalid_argument("fd to iostream is not implemented except gcc/msvc.");
    }

} // rlib
#endif
namespace rlib {
    template <>
    constexpr inline std::istream & fd_to_istream<STDIN_FILENO>() {
        return std::cin;
    }
    template <>
    constexpr inline std::ostream & fd_to_ostream<STDOUT_FILENO>() {
        return std::cout;
    }
    template <>
    constexpr inline std::ostream & fd_to_ostream<STDERR_FILENO>() {
        return std::cerr;
    }
}

namespace rlib{
    class [[deprecated]] FileDescriptorSet
    {
    public:
        FileDescriptorSet() : m_size(0), maxFileDescriptor(0) {FD_ZERO(&m_fds_data);}
        void push(fd FileDescriptor) {FD_SET(FileDescriptor, &m_fds_data); ++m_size; maxFileDescriptor = (maxFileDescriptor > FileDescriptor ? maxFileDescriptor : FileDescriptor);}
        void pop(fd FileDescriptor) {FD_CLR(FileDescriptor, &m_fds_data); --m_size;} //It will break maxFileDescriptor.(for performance reason).
        void clear() {FD_ZERO(&m_fds_data); m_size = 0;maxFileDescriptor = 0;}
        bool check(fd FileDescriptor) {return FD_ISSET(FileDescriptor, &m_fds_data);}
        size_t size() const {return m_size;}
        int getMaxFileDescriptor() const {return maxFileDescriptor;}
        fd_set *getptr() {return &m_fds_data;}
    private:
        fd_set m_fds_data;
        size_t m_size;
        int maxFileDescriptor;
    };
}
#endif
