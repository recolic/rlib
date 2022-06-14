#ifndef RLIB_META_HPP_
#define RLIB_META_HPP_

#include <rlib/sys/os.hpp>
#include <cstddef> // size_t
#include <tuple>

#include <type_traits>

namespace rlib {
    namespace impl {
        template<typename T>
        struct is_callable_helper {
        private:
            typedef char(&yes)[1];
            typedef char(&no)[2];

            struct Fallback { void operator()(); };
            struct Derived : T, Fallback { };

            template<typename U, U> struct Check;

            template<typename>
            static yes test(...);

            template<typename C>
            static no test(Check<void (Fallback::*)(), &C::operator()>*);

            static constexpr bool value = sizeof(test<Derived>(0)) == sizeof(yes);
        public:
            static constexpr bool real_value = std::conditional<std::is_class<T>::value, impl::is_callable_helper<T>, std::is_function<T>>::type::value;
        };
    }
    template<typename T>
    struct is_callable : public std::integral_constant<bool, impl::is_callable_helper<T>::real_value> {
    };
} // end namespace rlib


namespace rlib {
    #if RLIB_CXX_STD >= 2017
    namespace impl {
        template <auto first_ele, auto... _>
        struct array_first_ele_type_impl { using type = decltype(first_ele); };
    }
    template <auto... arr>
    struct meta_array {
        using this_type = typename ::rlib::meta_array<arr ...>;
        using element_type = typename ::rlib::impl::array_first_ele_type_impl<arr...>::type;

        template <size_t index>
        struct at_last {
            static constexpr auto value() noexcept {
                return at_last_impl<index, arr ...>::value();
            }
            constexpr operator element_type() {
                return at_last<index>::value();
            }
        };
        
        template <size_t index>
        struct at {
            static constexpr auto value() {
                return at_last<sizeof...(arr) - index - 1>::value();
            }
            constexpr operator element_type() {
                return at<index>::value();
            }
        };

        static constexpr auto to_tuple() {
            return std::make_tuple(arr ...);
        }

    private:
        template <size_t index, auto first_ele, auto... _arr>
        struct at_last_impl {
            static constexpr auto value() {
                if constexpr(sizeof...(_arr) == index)
                    return first_ele;
                else
                    return at_last_impl<index, _arr ...>::value();
            }
        };
    };
    #endif

    template <size_t... forwardedArgs> struct argForwarder {};

    namespace impl {
        template <typename T, std::size_t _>
        using get_T = T;
        template <typename T, std::size_t... _>
        auto make_N_tuple_impl_f(std::index_sequence<_ ...>) {
            using make_N_tuple_impl = std::tuple<get_T<T, _> ...>;
            return make_N_tuple_impl();
        }
    } // end namespace rlib::impl
    template <typename T, std::size_t N>
    using N_tuple_t = decltype(impl::make_N_tuple_impl_f<T>(std::make_integer_sequence<std::size_t, N>()));

} // end namespace rlib

#endif
