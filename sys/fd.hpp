#ifndef R_FDSET_HPP
#define R_FDSET_HPP

#include <unistd.h>
#include <sys/types.h>
#include <fstream>

#include <rlib/sys/os.hpp>
#if RLIB_OS_ID == OS_WINDOWS
using fd = HANDLE;
#else
using fd = int;
#endif

#if RLIB_COMPILER_ID == CC_GCC 
#include <ext/stdio_filebuf.h>
namespace rlib {
inline auto fd_to_ifstream(fd posix_handle) {
     __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in);
     return std::move(std::ifstream(&filebuf));
}
inline auto fd_to_ofstream(fd posix_handle) {
     __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::out);
     return std::move(std::ofstream(&filebuf));
}
inline auto fd_to_fstream(fd posix_handle) {
     __gnu_cxx::stdio_filebuf<char> filebuf(posix_handle, std::ios::in || std::ios::out);
     return std::move(std::fstream(&filebuf));
}



} // rlib
#elif RLIB_COMPILER_ID == CC_MSVC
namespace rlib {
inline auto fd_to_istream(fd posix_handle) {
    ifstream ifs(::_fdopen(posix_handle, "r"));
    return ifs.rdbuf();
}
} // rlib
#else
namespace rlib {
constexpr inline auto fd_to_istream(fd handle) {
    if constexpr(handle == STDIN_FILENO)
        return std::cin.rdbuf();
    else
        throw std::invalid_argument("filefd != 1 to stream is not implemented except gcc/msvc.");
}
} // rlib
#endif

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
