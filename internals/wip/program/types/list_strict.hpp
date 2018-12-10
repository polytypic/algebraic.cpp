#pragma once

#include "list.hpp"

#include "core/structures.hpp"

#include "basic.hpp"

#include "polyfill_v1/type_traits.hpp"

template <class X> list_t<std::remove_cvref_t<X>> list(X &&x) {
  return cons(std::forward<X>(x), list_t<std::remove_cvref_t<X>>(none));
}

template <class X, class... Xs>
list_t<std::remove_cvref_t<X>> list(X &&x, Xs &&... xs) {
  return cons(std::forward<X>(x), list(std::forward<Xs>(xs)...));
}

template <class V, class Action>
void for_each(Action &&action, const list_t<V> &vs) {
  for_each<shared_t>(action, vs);
}

template <class X> list_t<X> drop(int n, const list_t<X> &xs) {
  return drop<shared_t>(n, xs);
}

template <class X, class S, class SXS>
S fold_left(SXS &&sxs, const S &s, const list_t<X> &xs) {
  return fold_left<shared_t>(sxs, s, xs);
}

template <class X>
list_t<X> rev_append(const list_t<X> &to, const list_t<X> &from) {
  return fold_left([](auto &xs, auto &x) { return cons(x, xs); }, to, from);
}

template <class X> list_t<X> reverse(const list_t<X> &xs) {
  return rev_append(list_t<X>(none), xs);
}

template <class X, class XY, class Y = std::invoke_result_t<XY, const X &>>
list_t<Y> map_rev_append(const XY &xy, const list_t<Y> &ys,
                         const list_t<X> &xs) {
  return fold_left([&](auto &ys, auto &x) { return cons(xy(x), ys); }, ys, xs);
}

struct append {
  template <class X> using tycon_t = list_t<X>;

  template <class MX> using param_t = detail::list_of_t<MX>;

  template <class X> static tycon_t<std::remove_cvref_t<X>> pure(X &&x) {
    return list(std::forward<X>(x));
  }

  template <class X, class XYM,
            class Y = param_t<std::invoke_result_t<XYM, const X &>>>
  static tycon_t<Y> bind(XYM &&xyM, const tycon_t<X> &xM) {
    return reverse(
        fold_left([&](auto &ys, auto &x) { return rev_append(ys, xyM(x)); },
                  list_t<Y>(none), xM));
  }
};

template <class X> list_t<X> intersperse(const X &s, const list_t<X> &xs) {
  if (auto xxs = as<L(Some)>(xs))
    return cons(head(xxs),
                bind<append>([&](auto &x) { return list(s, x); }, tail(xxs)));
  return xs;
}
