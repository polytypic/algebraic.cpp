#pragma once

#include "list.hpp"

#include "polyfill_v1/type_traits.hpp"

template <class V, class Action>
void for_each(Action &&action, const list_t<V, lazy_t> &vs) {
  return for_each<lazy_t>(action, vs);
}

template <class V> list_t<V, lazy_t> take(int n, const list_t<V, lazy_t> &xs) {
  return thunk([=]() -> list_t<V, lazy_t> {
    if (n <= 0)
      return none;
    if (auto xxs = as<L(Some)>(xs))
      return cons(head(xxs), take(n - 1, tail(xxs)));
    return none;
  });
}

template <class V> list_t<V, lazy_t> drop(int n, const list_t<V, lazy_t> &xs) {
  return drop<lazy_t>(n, xs);
}

template <class Fn, class X, class Y,
          class Z = std::invoke_result_t<Fn, const X &, const Y &>>
list_t<Z, lazy_t> zip_with(const Fn &fn, const list_t<X, lazy_t> &xs,
                           const list_t<Y, lazy_t> &ys) {
  return thunk([=]() -> list_t<Z, lazy_t> {
    if (auto xxs = as<L(Some)>(xs))
      if (auto yys = as<L(Some)>(ys))
        return cons(fn(head(xxs), head(yys)),
                    zip_with(fn, tail(xxs), tail(yys)));
    return none;
  });
}
