#include <rlib/require/cxx11>
#include <streambuf>
#include <iostream>
#include <rlib/sys/os.hpp>

namespace rlib {
    namespace impl {
        class NullStreamBuf : public std::streambuf
        {
        public:
            int overflow(int c) { return c; }
        };
#if RLIB_CXX_STD < 2017
        extern NullStreamBuf null_streambuf;
#else
        inline NullStreamBuf null_streambuf;
#endif
    }
    
#if RLIB_CXX_STD < 2017
    extern std::ostream null_stream;
#else
    inline std::ostream null_stream(&impl::null_streambuf);
#endif
}
