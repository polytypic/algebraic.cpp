#pragma once

#include "cons.hpp"
#include "none.hpp"

template <class X, template <class Vs> class C = shared_t>
using list_t =
    fix_t<tycon_t<seq_t<Rec, Arg>, C<sum_t<none_t, cons_t<Arg, Rec>>>>, X>;

namespace detail {
template <class T> struct list_of_s;
template <class T> using list_of_t = tpnm<list_of_s<T>>;

template <class T> struct list_of_s<list_t<T, shared_t>> : is_type<T> {};
template <class T> struct list_of_s<list_t<T, lazy_t>> : is_type<T> {};
} // namespace detail

template <template <class Vs> class C, class V, class Action>
void for_each(Action &&action, const list_t<V, C> &vs) {
  if (auto vvs = as<L(Some)>(vs)) {
    action(head(vvs));
    return for_each<C>(action, tail(vvs));
  }
}

template <template <class Vs> class C, class X>
list_t<X, C> drop(int n, const list_t<X, C> &xs) {
  if (n <= 0)
    return xs;
  if (auto xxs = as<L(Some)>(xs))
    return drop(n - 1, tail(xxs));
  return xs;
}

template <template <class Vs> class C, class X, class S, class SXS>
S fold_left(SXS &&sxs, const S &s, const list_t<X, C> &xs) {
  if (auto xxs = as<L(Some)>(xs))
    return fold_left(sxs, sxs(s, head(xxs)), tail(xxs));
  return s;
}
