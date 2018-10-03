#include <rlib/require/cxx11>
#include <streambuf>
#include <iostream>
#include <rlib/sys/os.hpp>

// I'm testing this macro. if it works good, I'll apply it to rlib.
#if RLIB_CXX_STD >= 2017
#define RLIB_IMPL_VAR_DECL inline
#else
#define RLIB_IMPL_VAR_DECL extern
#endif

namespace rlib {
    namespace impl {
        class NullStreamBuf : public std::streambuf
        {
        public:
            int overflow(int c) { return c; }
        };
        RLIB_IMPL_VAR_DECL NullStreamBuf null_streambuf;
    }
    
    RLIB_IMPL_VAR_DECL std::ostream null_stream(&impl::null_streambuf);
}
