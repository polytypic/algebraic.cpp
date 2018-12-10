#pragma once

#include "some.hpp"

template <class V, class Vs>
using cons_t = some_t<product_t<the_t<L(Head), V>, the_t<L(Tail), Vs>>>;

template <class C> auto &head(const C &c) { return get<L(Head)>(c); }

template <class C> auto &tail(const C &c) { return get<L(Tail)>(c); }

template <class V, class Vs> std::remove_cvref_t<Vs> cons(V &&v, Vs &&vs) {
  return some(product(the<L(Head)>(std::forward<V>(v)),
                      the<L(Tail)>(std::forward<Vs>(vs))));
}
