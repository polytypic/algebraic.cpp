#pragma once

#include "polyfill_v1/type_traits.hpp"

// tpnm<T>

template <class T> using tpnm = typename T::type;

// is_type<T>::type

template <class T> struct is_type { using type = T; };

// is_value<T, v>::value

template <class T, T v> struct is_value : is_type<T> {
  static const T value = v;
};

// if_t< C, T, E >

template <bool c, class T, class E> struct if_s : is_type<E> {};

template <class T, class E> struct if_s<true, T, E> : is_type<T> {};

template <class C, class T, class E> using if_t = tpnm<if_s<C::value, T, E>>;

// seq_t<Ts...>

template <class... Ts> struct seq_t : is_type<seq_t<Ts...>> {};

// map_t<F, S>

template <template <class> class F, class S> struct map_s;

template <template <class> class F, class S> using map_t = tpnm<map_s<F, S>>;

template <template <class> class F, class... Ts>
struct map_s<F, seq_t<Ts...>> : seq_t<F<Ts>...> {};

// contains_t<T, S>::value

template <class T, class S> struct contains_s;

template <class T, class S>
using contains_t = is_value<bool, contains_s<T, S>::value>;

template <class T> struct contains_s<T, seq_t<>> : is_value<bool, false> {};

template <class T, class... Ts>
struct contains_s<T, seq_t<T, Ts...>> : is_value<bool, true> {};

template <class T, class T0, class... Ts>
struct contains_s<T, seq_t<T0, Ts...>> : contains_t<T, seq_t<Ts...>> {};

// and_t<L, R>::value

template <class L, class R> using and_t = is_value<bool, L::value && R::value>;

// or_t<L, R>::value

template <class L, class R> using or_t = is_value<bool, L::value || R::value>;

// has_duplicates_t<S>::value

template <class S> struct has_duplicates_s;

template <class S>
using has_duplicates_t = is_value<bool, has_duplicates_s<S>::value>;

template <> struct has_duplicates_s<seq_t<>> : is_value<bool, false> {};

template <class T, class... Ts>
struct has_duplicates_s<seq_t<T, Ts...>>
    : or_t<contains_t<T, seq_t<Ts...>>, has_duplicates_t<seq_t<Ts...>>> {};

// is_subset_of_t<Super, Sub>::value

template <class L, class R> struct is_subset_of_s;

template <class L, class R>
using is_subset_of_t = is_value<bool, is_subset_of_s<L, R>::value>;

template <class L> struct is_subset_of_s<L, seq_t<>> : is_value<bool, true> {};

template <class L, class R0, class... Rs>
struct is_subset_of_s<L, seq_t<R0, Rs...>>
    : and_t<contains_t<R0, L>, is_subset_of_t<L, seq_t<Rs...>>> {};

// when_subset_of_t<Super, Sub>

template <class L, class R>
using when_subset_of_t = std::enable_if_t<is_subset_of_t<L, R>::value>;

// unless_contains_ref_of_t<V, Ts...>

template <class V, class... Ts>
using unless_contains_t =
    std::enable_if_t<!contains_t<V, seq_t<std::remove_cvref_t<Ts>...>>::value>;
