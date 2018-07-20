#include <rlib/log.hpp> //log_level_t
#include <sstream>
namespace rlib {
    namespace impl {
        bool enable_endl_flush = true;
        int max_predefined_log_level = (int)log_level_t::DEBUG;
        thread_local std::stringstream _format_string_helper_ss;
        thread_local std::stringstream to_string_by_sstream_ss;        
    }
}
