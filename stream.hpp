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
        // stdc++ 17 removed
        extern NullStreamBuf null_streambuf;
    }
    
//#if RLIB_CXX_STD < 2017
    extern std::ostream null_stream;
//#else
//    inline std::ostream null_stream(&impl::null_streambuf);
//#endif
}
