#pragma once

#include "none.hpp"
#include "some.hpp"

template <class V> using option_t = sum_t<none_t, some_t<V>>;

namespace detail {
template <class T> struct option_of_s;
template <class T> struct option_of_s<option_t<T>> : is_type<T> {};
template <class T> using option_of_t = tpnm<option_of_s<T>>;
} // namespace detail

struct option {
  template <class X> using tycon_t = option_t<X>;

  template <class MX> using param_t = detail::option_of_t<MX>;

  template <class X> static tycon_t<X> pure(X &&x) {
    return some(std::forward<X>(x));
  }

  template <class X, class XYM,
            class Y = param_t<std::invoke_result_t<XYM, const X &>>>
  static tycon_t<Y> bind(XYM &&xyM, const tycon_t<X> &xM) {
    if (auto xS = as<L(Some)>(xM))
      return xyM(it(xS));
    return none;
  }

  template <class X> static tycon_t<X> zero() { return tycon_t<X>(none); }

  template <class X>
  static const tycon_t<X> &alt(const tycon_t<X> &lhs, const tycon_t<X> &rhs) {
    return as<L(Some)>(lhs) ? lhs : rhs;
  }
};
