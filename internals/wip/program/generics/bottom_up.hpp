#pragma once

#include "core/types.hpp"

template <class A, class AB, class O, class = unless_contains_t<A, O>>
auto &map_this(const AB &, const O &o) {
  return o;
}

template <class A, class AB> auto map_this(const AB &ab, const A &a) {
  return ab(a);
}

//

template <class F, class T, class V>
auto map_children(const F &f, const the_t<T, V> &t) {
  return the_t<T, std::invoke_result_t<F, const V &>>(f(it(t)));
}

template <class F, class... Ts, class... Vs>
auto map_children(const F &f, const product_t<the_t<Ts, Vs>...> &p) {
  return product(map_children(f, static_cast<const the_t<Ts, Vs> &>(p))...);
}

template <class F, class... Ts, class... Vs>
auto map_children(const F &f, const sum_t<the_t<Ts, Vs>...> &s) {
  return s.map(f);
}

template <class F, template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
auto map_children(const F &f, const rec_t<TyCon, TyArgs...> &r) {
  return rec_t<TyCon, TyArgs...>(map_children(f, unwrap(r)));
}

template <class F, class... Es>
auto map_children(const F &&f, const shared_t<sum_t<Es...>> &s) {
  return s.map(f);
}

template <class F, class X, class Y = std::invoke_result_t<F, const X &>>
shared_t<Y> map_children(const F &f, const shared_t<X> &s) {
  return f(it(s));
}

template <class F, class X> const X &map_children(const F &, const X &x) {
  return x;
}

//

template <class A, class AB, class V>
auto bottom_up_at(const AB &ab, const V &v) {
  return map_this<A>(
      ab, map_children([&](auto &c) { return bottom_up_at<A>(ab, c); }, v));
}

template <class Ab, class V> auto bottom_up(const Ab &ab, const V &v) {
  return bottom_up_at<V>(ab, v);
}
