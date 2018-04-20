#ifndef RLIB_LOG_HPP_
#define RLIB_LOG_HPP_ 1

#include <string>
#include <fstream>
#include <list>
#include <climits>
#include <rlib/sys/os.hpp>
#include <rlib/sys/fd.hpp>
#include <rlib/stdio.hpp>

// currently disable this error-prone shit.
#define RLIB_ENABLE_LOGGER_FROM_FD 0

#ifndef RLIB_ENABLE_LOGGER_FROM_FD
#if RLIB_OS_ID != OS_UNKNOWN
#   if RLIB_COMPILER_ID == CC_GCC
#       include <ext/stdio_filebuf.h>
#       define RLIB_ENABLE_LOGGER_FROM_FD 1
#   elif RLIB_COMPILER_ID == CC_MSVC
#       define RLIB_ENABLE_LOGGER_FROM_FD 1
#   endif
#endif
#endif

namespace rlib {
    using namespace rlib::literals;

    // Allow extension.
    enum class log_level_t : int { FATAL = 1, ERROR, WARNING, INFO, VERBOSE, DEBUG };
    namespace impl { extern int max_predefined_log_level; }
    /*
    How to update log_level_t:
        Extend `enum log_level_t ...`
        Modify libr.cc:`max_predefined_log_level ...`
        Add an RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND
        Append logger::predefined_log_level_name
    */

    class logger {
    public:
        logger() = delete;
        logger(std::ostream &stream) : stream(stream) {}
        logger(const std::string &file_name) : stream(*new std::ofstream(file_name, std::ios::out)), 
            must_delete_stream_as_ofstream(true) {}
        ~logger() {
            if(must_delete_stream_as_ofstream)
                delete dynamic_cast<std::ofstream *>(&stream);
        }
        
#if RLIB_ENABLE_LOGGER_FROM_FD == 1
#if RLIB_OS_ID != OS_UNKNOWN
        logger(fd file_descriptor_or_handle)
#   if RLIB_COMPILER_ID == CC_GCC
        : _gcc_filebuf(file_descriptor_or_handle, std::ios::out), _gcc_real_stream(&_gcc_filebuf), 
            stream(_gcc_real_stream) {}
#   elif RLIB_COMPILER_ID == CC_MSVC
        : _msvc_real_stream(::_fdopen(file_descriptor_or_handle, "w")), 
            stream(_msvc_real_stream) {}
#   endif
#endif
#endif

        void set_flush(bool enable_flush) noexcept {
            this->enable_flush = enable_flush;
        }
        void log(const std::string &info, log_level_t level = log_level_t::INFO) const {
            stream << "[{}]{}"_format(log_level_name(level), info) << RLIB_IMPL_ENDLINE;
            if(enable_flush)
                stream.flush();
        }
        // Warning: this method is not thread-safe.
        log_level_t register_log_level(const std::string &name) {
            if(impl::max_predefined_log_level == INT_MAX)
                throw std::overflow_error("At most {}(INT_MAX) log_level is allowed."_format(INT_MAX));
            ++ impl::max_predefined_log_level;
            log_level_t new_level = (log_level_t)impl::max_predefined_log_level;
            custom_log_level_names.push_back({new_level, name});
            return new_level;
        }

#define RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(_name, _enum_name) void _name(const std::string &info) const { \
            log(info, log_level_t::_enum_name); }

        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(fatal, FATAL)
        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(error, ERROR)
        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(warn, WARNING)
        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(info, INFO)
        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(verbose_info, VERBOSE)
        RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND(debug, DEBUG)

#undef RLIB_IMPL_MACRO_LOG_ADD_SHORTHAND

    private:
        static constexpr const char * predefined_log_level_name(log_level_t level) noexcept {
            switch(level) {
            case log_level_t::FATAL:
                return "FATAL";
            case log_level_t::ERROR:
                return "ERROR";
            case log_level_t::WARNING:
                return "WARNING";
            case log_level_t::INFO:
                return "INFO";
            case log_level_t::VERBOSE:
                return "VERBOSE";
            case log_level_t::DEBUG:
                return "DEBUG";
            default:
                return "";
            }
        }
        std::string log_level_name(log_level_t level) const noexcept {
            std::string name = predefined_log_level_name(level);
            if(!name.empty())
                return std::move(name);
            for(const auto &level_and_name : custom_log_level_names) {
                if(level == level_and_name.first) {
                    name = level_and_name.second;
                    break;
                }
            }
            if(!name.empty())
                return std::move(name);
            name = "LEVEL-";
            name += std::to_string((int)level);
            return std::move(name);
        }

        std::list<std::pair<log_level_t, std::string> > custom_log_level_names;

        std::ostream &stream;
        bool must_delete_stream_as_ofstream = false;
        bool enable_flush = true;
#if RLIB_ENABLE_LOGGER_FROM_FD == 1
#   if RLIB_COMPILER_ID == CC_GCC
        __gnu_cxx::stdio_filebuf<char> _gcc_filebuf;
        std::ostream _gcc_real_stream;
#   elif RLIB_COMPILER_ID == CC_MSVC
        std::ofstream _msvc_real_stream;
#   endif
#endif
    };
}

#endif