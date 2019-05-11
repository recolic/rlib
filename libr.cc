#include <rlib/log.hpp> //log_level_t
#include <rlib/stream.hpp>
#include <sstream>

#if (RLIB_CXX_STD >= 2017) && (RLIB_COMPILER_ID != CC_MSVC)
#warning library should not be compiled under C++17. Or The library won't work for c++14 users.
#endif

namespace rlib {
    namespace impl {
// If libr.cc is built under C++17, and other header files are included in C++14 project,
// Then something wrong will happen. So DO NOT ignore the following definitions. NEVER.
//#if RLIB_CXX_STD < 2017
        bool enable_endl_flush = true;
        int max_predefined_log_level = (int)log_level_t::DEBUG;
        NullStreamBuf null_streambuf;
//#endif
//#ifndef RLIB_MINGW_DISABLE_TLS
//#if RLIB_CXX_STD < 2017
        thread_local std::stringstream _format_string_helper_ss;
        thread_local std::stringstream to_string_by_sstream_ss;        
//#endif
//#endif
    }
//#if RLIB_CXX_STD < 2017
    std::ostream null_stream(&impl::null_streambuf);
//#endif
}
