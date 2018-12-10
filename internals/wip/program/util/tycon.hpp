#pragma once

#include "meta.hpp"

template <class Vs, class TyExp> struct tycon_t;

namespace detail {
// subst<V, T, E>

template <class V, class T, class E> struct subst_s : is_type<E> {};

template <class V, class T, class E> using subst = tpnm<subst_s<V, T, E>>;

//

template <class V, class T> struct subst_s<V, T, V> : is_type<T> {};

template <class V, class T, class E>
struct subst_s<V, T, tycon_t<V, E>> : is_type<tycon_t<V, E>> {};

template <class V, class T, template <class... Ps> class F, class... As>
struct subst_s<V, T, F<As...>> : is_type<F<subst<V, T, As>...>> {};

template <class V, class T,
          template <template <class... Ps> class F, class... As> class R,
          template <class... Ps> class F, class... As>
struct subst_s<V, T, R<F, As...>> : is_type<R<F, subst<V, T, As>...>> {};

template <class V, class T, class R, class... Ps>
struct subst_s<V, T, R(Ps...)> : is_type<subst<V, T, R>(subst<V, T, Ps>...)> {};
} // namespace detail

template <class V1, class TyExp>
struct tycon_t<seq_t<V1>, TyExp> : is_type<tycon_t<V1, TyExp>> {
  template <class T1> using F = detail::subst<V1, T1, TyExp>;
};

template <class V1, class V2, class TyExp>
struct tycon_t<seq_t<V1, V2>, TyExp> : is_type<tycon_t<seq_t<V1, V2>, TyExp>> {
  template <class T1, class T2>
  using F = detail::subst<V2, T2, detail::subst<V1, T1, TyExp>>;
};
