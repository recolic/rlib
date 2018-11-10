#include <rlib/log.hpp> //log_level_t
#include <rlib/stream.hpp>
#include <sstream>
namespace rlib {
    namespace impl {
#if RLIB_CXX_STD < 2017
        bool enable_endl_flush = true;
        int max_predefined_log_level = (int)log_level_t::DEBUG;
        NullStreamBuf null_streambuf;
#endif
#if RLIB_CXX_STD < 2017 || RLIB_COMPILER_ID == ICC || defined(RLIB_MINGW_DISABLE_INLINE_TLS)
        thread_local std::stringstream _format_string_helper_ss;
        thread_local std::stringstream to_string_by_sstream_ss;        
#endif
    }
#if RLIB_CXX_STD < 2017
    std::ostream null_stream(&impl::null_streambuf);
#endif
}
