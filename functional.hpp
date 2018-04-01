#ifndef RLIB_FUNCTIONAL_HPP_
#define RLIB_FUNCTIONAL_HPP_

#include <rlib/require/cxx14>
#include <rlib/class_decorator.hpp>

#include <type_traits>
#include <list>
#include <functional>
#include <chrono>

namespace rlib {
    namespace impl {
        template <typename Func, typename... Args>
        struct repeated_func {
            using return_type = typename std::invoke_result<Func, Args ...>::type;
            auto operator ()(size_t count, Func f, Args ... args) {
                for(size_t cter = 0; cter < count - 1; ++cter)
                    f(std::forward<Args>(args) ...);
                return f(std::forward<Args>(args) ...);
            }
        };
    }
}

namespace rlib {

    template <class Func, typename... Args>
    static inline double timed_func(::std::function<Func> f, Args && ... args)
    {
        auto begin = std::chrono::high_resolution_clock::now();
        f(std::forward<Args>(args) ...);
        auto end = std::chrono::high_resolution_clock::now();
        return ::std::chrono::duration<double>(end - begin).count(); 
    }

    template <class Func, typename... Args>
    static inline auto repeat(size_t count, Func && f, Args && ... args)
    {
        //using return_type = typename std::invoke_result<Func, Args ...>::type;
        //using return_type2 = typename std::invoke_result<typename impl::repeated_func<Func, Args ...>, size_t, Func, Args ...>::type;
        //using return_type3 = decltype(impl::repeated_func<Func, Args ...>()(count, f, args ...));
        //static_assert(std::is_same<return_type, return_type2>::value);
        //static_assert(std::is_same<return_type, return_type3>::value);

        return std::bind(impl::repeated_func<Func, Args ...>(), count, std::forward<Func>(f), std::forward<Args>(args) ...);
    }
    template <class Func, typename... Args>
    static inline ::std::list<typename ::std::result_of<Func(Args ...)>::type> repeat_and_return_list(size_t count, Func f, Args... args)
    {
        ::std::list<typename ::std::result_of<Func(Args ...)>::type> ret;
        for(size_t cter = 0; cter < count; ++cter)
            ret.push_back(std::move(f(args ...)));
        return std::move(ret);
    }
}
#endif
