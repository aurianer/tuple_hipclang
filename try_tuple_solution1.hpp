//  Copyright (c) 2011-2013 Thomas Heller
//  Copyright (c) 2011-2013 Hartmut Kaiser
//  Copyright (c) 2013-2015 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "always_void.hpp"
#include "pack.hpp"

#include <algorithm>
#include <array>
#include <cstddef>    // for size_t
#include <tuple>
#include <type_traits>
#include <utility>

#include <hip/hip_runtime.h>

#if defined(HPX_MSVC_WARNING_PRAGMA)
#pragma warning(push)
#pragma warning(disable : 4520)    // multiple default constructors specified
#endif

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

namespace hpx {
    template <typename... Ts>
    class tuple;

    template <typename T>
    struct tuple_size;    // undefined

    template <std::size_t I, typename T>
    struct tuple_element;    // undefined

    // Hide implementations of get<> inside an internal namespace to be able to
    // import those into the namespace std below without pulling in all of
    // hpx::util.
    namespace adl_barrier {

        template <std::size_t I, typename Tuple,
            typename Enable = typename util::always_void<
                typename tuple_element<I, Tuple>::type>::type>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type&
            get(Tuple& t) noexcept;

        template <std::size_t I, typename Tuple,
            typename Enable = typename util::always_void<
                typename tuple_element<I, Tuple>::type>::type>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type const&
            get(Tuple const& t) noexcept;

        template <std::size_t I, typename Tuple,
            typename Enable = typename util::always_void<typename tuple_element<
                I, typename std::decay<Tuple>::type>::type>::type>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type&&
            get(Tuple&& t) noexcept;

        template <std::size_t I, typename Tuple,
            typename Enable = typename util::always_void<
                typename tuple_element<I, Tuple>::type>::type>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type const&&
            get(Tuple const&& t) noexcept;
    }    // namespace adl_barrier

    // we separate the implementation of get for our tuple type so that
    // it can be injected into the std:: namespace
    namespace std_adl_barrier {

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type&
            get(tuple<Ts...>& t) noexcept;

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type const&
            get(tuple<Ts...> const& t) noexcept;

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type&&
            get(tuple<Ts...>&& t) noexcept;

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type const&&
            get(tuple<Ts...> const&& t) noexcept;
    }    // namespace std_adl_barrier

    using hpx::adl_barrier::get;
    using hpx::std_adl_barrier::get;
}    // namespace hpx


namespace hpx {

    namespace detail {

        ///////////////////////////////////////////////////////////////////////
        struct ignore_type
        {
            template <typename T>
            void operator=(T&& t) const
            {
            }
        };
    }    // namespace detail

    // 20.4.2, class template tuple
    template <>
    class tuple<>
    {
    public:
        // 20.4.2.1, tuple construction

        // constexpr tuple();
        // Value initializes each element.
        constexpr __host__ __device__ tuple() {}

        // tuple(const tuple& u) = default;
        // Initializes each element of *this with the corresponding element
        // of u.
        constexpr tuple(tuple const& /*other*/) = default;

        // tuple(tuple&& u) = default;
        // For all i, initializes the ith element of *this with
        // std::forward<Ti>(get<i>(u)).
        constexpr tuple(tuple&& /*other*/) = default;

        // 20.4.2.2, tuple assignment

        // tuple& operator=(const tuple& u);
        // Assigns each element of u to the corresponding element of *this.
        __host__ __device__ tuple& operator=(tuple const& /*other*/) noexcept
        {
            return *this;
        }

        // tuple& operator=(tuple&& u) noexcept(see below );
        // For all i, assigns std::forward<Ti>(get<i>(u)) to get<i>(*this).
        __host__ __device__ tuple& operator=(tuple&& /*other*/) noexcept
        {
            return *this;
        }

        // 20.4.2.3, tuple swap

        // void swap(tuple& rhs) noexcept(see below);
        // Calls swap for each element in *this and its corresponding element
        // in rhs.
        __host__ __device__ void swap(tuple& /*other*/) noexcept {}

    };

    // 20.4.2.5, tuple helper classes

    // template <class Tuple>
    // class tuple_size
    template <class T>
    struct tuple_size
    {
    };

    template <class T>
    struct tuple_size<const T> : tuple_size<T>
    {
    };

    template <class T>
    struct tuple_size<volatile T> : tuple_size<T>
    {
    };

    template <class T>
    struct tuple_size<const volatile T> : tuple_size<T>
    {
    };

    template <typename... Ts>
    struct tuple_size<tuple<Ts...>>
      : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    template <typename T0, typename T1>
    struct tuple_size<std::pair<T0, T1>>
      : std::integral_constant<std::size_t, 2>
    {
    };

    template <typename Type, std::size_t Size>
    struct tuple_size<std::array<Type, Size>>
      : std::integral_constant<std::size_t, Size>
    {
    };

    // template <size_t I, class Tuple>
    // class tuple_element
    template <std::size_t I, typename T>
    struct tuple_element
    {
    };

    template <std::size_t I, typename T>
    struct tuple_element<I, const T>
      : std::add_const<typename tuple_element<I, T>::type>
    {
    };

    template <std::size_t I, typename T>
    struct tuple_element<I, volatile T>
      : std::add_volatile<typename tuple_element<I, T>::type>
    {
    };

    template <std::size_t I, typename T>
    struct tuple_element<I, const volatile T>
      : std::add_cv<typename tuple_element<I, T>::type>
    {
    };

    template <std::size_t I, typename... Ts>
    struct tuple_element<I, tuple<Ts...>>
    {
        using type = typename util::at_index<I, Ts...>::type;

        static constexpr __host__ __device__ inline type& get(
            tuple<Ts...>& tuple) noexcept
        {
            return tuple.template get<I>();
        }

        static constexpr __host__ __device__ inline type const& get(
            tuple<Ts...> const& tuple) noexcept
        {
            return tuple.template get<I>();
        }
    };

    template <typename T0, typename T1>
    struct tuple_element<0, std::pair<T0, T1>>
    {
        using type = T0;

        static constexpr __host__ __device__ inline type& get(
            std::pair<T0, T1>& tuple) noexcept
        {
            return tuple.first;
        }

        static constexpr __host__ __device__ inline type const& get(
            std::pair<T0, T1> const& tuple) noexcept
        {
            return tuple.first;
        }
    };

    template <typename T0, typename T1>
    struct tuple_element<1, std::pair<T0, T1>>
    {
        using type = T1;

        static constexpr __host__ __device__ inline type& get(
            std::pair<T0, T1>& tuple) noexcept
        {
            return tuple.second;
        }

        static constexpr __host__ __device__ inline type const& get(
            std::pair<T0, T1> const& tuple) noexcept
        {
            return tuple.second;
        }
    };

    template <std::size_t I, typename Type, std::size_t Size>
    struct tuple_element<I, std::array<Type, Size>>
    {
        using type = Type;

        static constexpr __host__ __device__ inline type& get(
            std::array<Type, Size>& tuple) noexcept
        {
            return tuple[I];
        }

        static constexpr __host__ __device__ inline type const& get(
            std::array<Type, Size> const& tuple) noexcept
        {
            return tuple[I];
        }
    };

    template <std::size_t I, typename Type, std::size_t Size>
    struct tuple_element<I, std::array<Type, Size> const>
    {
        using type = Type;

        static constexpr __host__ __device__ inline type& get(
            std::array<Type, Size>& tuple) noexcept
        {
            return tuple[I];
        }

        static constexpr __host__ __device__ inline type const& get(
            std::array<Type, Size> const& tuple) noexcept
        {
            return tuple[I];
        }
    };

    // 20.4.2.6, element access
    namespace adl_barrier {

        // template <size_t I, class... Types>
        // constexpr typename tuple_element<I, tuple<Types...> >::type&
        // get(tuple<Types...>& t) noexcept;
        template <std::size_t I, typename Tuple, typename Enable>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type&
            get(Tuple& t) noexcept
        {
            return tuple_element<I, Tuple>::get(t);
        }

        // template <size_t I, class... Types>
        // constexpr typename tuple_element<I, tuple<Types...> >::type const&
        // get(const tuple<Types...>& t) noexcept;
        template <std::size_t I, typename Tuple, typename Enable>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type const&
            get(Tuple const& t) noexcept
        {
            return tuple_element<I, Tuple>::get(t);
        }

        // template <size_t I, class... Types>
        // constexpr typename tuple_element<I, tuple<Types...> >::type&&
        // get(tuple<Types...>&& t) noexcept;
        template <std::size_t I, typename Tuple, typename Enable>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type&&
            get(Tuple&& t) noexcept
        {
            return std::forward<typename tuple_element<I, Tuple>::type>(
                get<I>(t));
        }

        // template <size_t I, class... Types>
        // constexpr typename tuple_element<I, tuple<Types...> >::type const&&
        // get(const tuple<Types...>&& t) noexcept;
        template <std::size_t I, typename Tuple, typename Enable>
        constexpr __host__ __device__ inline
            typename tuple_element<I, Tuple>::type const&&
            get(Tuple const&& t) noexcept
        {
            return std::forward<typename tuple_element<I, Tuple>::type const>(
                get<I>(t));
        }
    }    // namespace adl_barrier

    namespace std_adl_barrier {

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type&
            get(tuple<Ts...>& t) noexcept
        {
            return tuple_element<I, tuple<Ts...>>::get(t);
        }

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type const&
            get(tuple<Ts...> const& t) noexcept
        {
            return tuple_element<I, tuple<Ts...>>::get(t);
        }

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type&&
            get(tuple<Ts...>&& t) noexcept
        {
            return std::forward<typename tuple_element<I, tuple<Ts...>>::type>(
                get<I>(t));
        }

        template <std::size_t I, typename... Ts>
        constexpr __host__ __device__ inline
            typename tuple_element<I, tuple<Ts...>>::type const&&
            get(tuple<Ts...> const&& t) noexcept
        {
            return std::forward<
                typename tuple_element<I, tuple<Ts...>>::type const>(get<I>(t));
        }
    }    // namespace std_adl_barrier

    // 20.4.2.4, tuple creation functions
    detail::ignore_type const ignore = {};

    // template<class... Types>
    // tuple<Types&&...> forward_as_tuple(Types&&... t) noexcept;
    // Constructs a tuple of references to the arguments in t suitable for
    // forwarding as arguments to a function. Because the result may contain
    // references to temporary variables, a program shall ensure that the
    // return value of this function does not outlive any of its arguments.
    template <typename... Ts>
    __host__ __device__ inline tuple<Ts&&...> forward_as_tuple(
        Ts&&... vs) noexcept
    {
        return tuple<Ts&&...>(std::forward<Ts>(vs)...);
    }

    // template<class... Types>
    // tuple<Types&...> tie(Types&... t) noexcept;
    template <typename... Ts>
    __host__ __device__ inline tuple<Ts&...> tie(Ts&... vs) noexcept
    {
        return tuple<Ts&...>(vs...);
    }

    //template <class... Tuples>
    //constexpr tuple<Ctypes ...> tuple_cat(Tuples&&...);
    namespace detail {

        /// Deduces to the overall size of all given tuples
        template <std::size_t Size, typename Tuples>
        struct tuple_cat_size_impl;

        template <std::size_t Size>
        struct tuple_cat_size_impl<Size, util::pack<>>
          : std::integral_constant<std::size_t, Size>
        {
        };

        template <std::size_t Size, typename Head, typename... Tail>
        struct tuple_cat_size_impl<Size, util::pack<Head, Tail...>>
          : tuple_cat_size_impl<(Size + tuple_size<Head>::value),
                util::pack<Tail...>>
        {
        };

        template <typename... Tuples>
        struct tuple_cat_size : tuple_cat_size_impl<0, util::pack<Tuples...>>
        {
        };

        ///////////////////////////////////////////////////////////////////////
        template <std::size_t I, typename Tuples, typename Enable = void>
        struct tuple_cat_element;

        template <std::size_t I, typename Head, typename... Tail>
        struct tuple_cat_element<I, util::pack<Head, Tail...>,
            typename std::enable_if<(I < tuple_size<Head>::value)>::type>
          : tuple_element<I, Head>
        {
            template <typename THead, typename... TTail>
            static constexpr __host__ __device__ inline auto get(
                THead&& head, TTail&&... /*tail*/) noexcept
                -> decltype(hpx::get<I>(std::forward<THead>(head)))
            {
                return hpx::get<I>(std::forward<THead>(head));
            }
        };

        template <std::size_t I, typename Head, typename... Tail>
        struct tuple_cat_element<I, util::pack<Head, Tail...>,
            typename std::enable_if<(I >= tuple_size<Head>::value)>::type>
          : tuple_cat_element<I - tuple_size<Head>::value, util::pack<Tail...>>
        {
            using _members = tuple_cat_element<I - tuple_size<Head>::value,
                util::pack<Tail...>>;

            template <typename THead, typename... TTail>
            static constexpr __host__ __device__ inline auto get(
                THead&& /*head*/, TTail&&... tail) noexcept
                -> decltype(_members::get(std::forward<TTail>(tail)...))
            {
                return _members::get(std::forward<TTail>(tail)...);
            }
        };

        ///////////////////////////////////////////////////////////////////////
        template <typename Indices, typename Tuples>
        struct tuple_cat_result_impl;

        template <std::size_t... Is, typename... Tuples>
        struct tuple_cat_result_impl<util::index_pack<Is...>,
            util::pack<Tuples...>>
        {
            using type = tuple<
                typename tuple_cat_element<Is, util::pack<Tuples...>>::type...>;
        };

        template <typename Indices, typename Tuples>
        using tuple_cat_result_of_t =
            typename tuple_cat_result_impl<Indices, Tuples>::type;

        template <std::size_t... Is, typename... Tuples, typename... Tuples_>
        constexpr __host__ __device__ inline auto tuple_cat_impl(
            util::index_pack<Is...> is_pack, util::pack<Tuples...> tuple_pack,
            Tuples_&&... tuples)
            -> tuple_cat_result_of_t<decltype(is_pack), decltype(tuple_pack)>
        {
            return tuple_cat_result_of_t<decltype(is_pack),
                decltype(tuple_pack)>{
                tuple_cat_element<Is, util::pack<Tuples...>>::get(
                    std::forward<Tuples_>(tuples)...)...};
        }
    }    // namespace detail

    template <typename... Tuples>
    constexpr __host__ __device__ inline auto tuple_cat(Tuples&&... tuples)
        -> decltype(detail::tuple_cat_impl(
            typename util::make_index_pack<detail::tuple_cat_size<
                typename std::decay<Tuples>::type...>::value>::type{},
            util::pack<typename std::decay<Tuples>::type...>{},
            std::forward<Tuples>(tuples)...))
    {
        return detail::tuple_cat_impl(
            typename util::make_index_pack<detail::tuple_cat_size<
                typename std::decay<Tuples>::type...>::value>::type{},
            util::pack<typename std::decay<Tuples>::type...>{},
            std::forward<Tuples>(tuples)...);
    }

    // 20.4.2.7, relational operators

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator==
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    // The elementary comparisons are performed in order from the zeroth index
    // upwards. No comparisons or element accesses are performed after the
    // first equality comparison that evaluates to false.
    namespace detail {
        template <std::size_t I, std::size_t Size>
        struct tuple_equal_to
        {
            template <typename TTuple, typename UTuple>
            static constexpr __host__ __device__ inline bool call(
                TTuple const& t, UTuple const& u)
            {
                return get<I>(t) == get<I>(u) &&
                    tuple_equal_to<I + 1, Size>::call(t, u);
            }
        };

        template <std::size_t Size>
        struct tuple_equal_to<Size, Size>
        {
            template <typename TTuple, typename UTuple>
            static constexpr __host__ __device__ inline bool call(
                TTuple const&, UTuple const&)
            {
                return true;
            }
        };
    }    // namespace detail

    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator==(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return detail::tuple_equal_to<0, sizeof...(Ts)>::call(t, u);
    }

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator!=
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator!=(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return !(t == u);
    }

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator<
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    // The result is defined as: (bool)(get<0>(t) < get<0>(u)) ||
    // (!(bool)(get<0>(u) < get<0>(t)) && ttail < utail), where rtail for some
    // tuple r is a tuple containing all but the first element of r. For any
    // two zero-length tuples e and f, e < f returns false.
    namespace detail {
        template <std::size_t I, std::size_t Size>
        struct tuple_less_than
        {
            template <typename TTuple, typename UTuple>
            static constexpr __host__ __device__ inline bool call(
                TTuple const& t, UTuple const& u)
            {
                return get<I>(t) < get<I>(u) ||
                    (!(get<I>(u) < get<I>(t)) &&
                        tuple_less_than<I + 1, Size>::call(t, u));
            }
        };

        template <std::size_t Size>
        struct tuple_less_than<Size, Size>
        {
            template <typename TTuple, typename UTuple>
            static constexpr __host__ __device__ inline bool call(
                TTuple const& t, UTuple const& u)
            {
                return false;
            }
        };
    }    // namespace detail

    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator<(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return detail::tuple_less_than<0, sizeof...(Ts)>::call(t, u);
    }

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator>
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator>(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return u < t;
    }

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator<=
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator<=(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return !(u < t);
    }

    // template<class... TTypes, class... UTypes>
    // constexpr bool operator>=
    //     (const tuple<TTypes...>& t, const tuple<UTypes...>& u);
    template <typename... Ts, typename... Us>
    constexpr __host__ __device__ inline
        typename std::enable_if<sizeof...(Ts) == sizeof...(Us), bool>::type
        operator>=(tuple<Ts...> const& t, tuple<Us...> const& u)
    {
        return !(t < u);
    }

    // 20.4.2.9, specialized algorithms

    // template <class... Types>
    // void swap(tuple<Types...>& x, tuple<Types...>& y) noexcept(x.swap(y));
    // x.swap(y)
    template <typename... Ts>
    __host__ __device__ inline void swap(
        tuple<Ts...>& x, tuple<Ts...>& y) noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }

}    // namespace hpx


#if defined(HPX_MSVC_WARNING_PRAGMA)
#pragma warning(pop)
#endif

#if defined(__clang__)
#pragma clang diagnostic pop
#endif
