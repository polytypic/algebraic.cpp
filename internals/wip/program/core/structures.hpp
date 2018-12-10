#pragma once

#include "polyfill_v1/type_traits.hpp"
#include <utility>

template <class C, class X> using apply_t = typename C::template tycon_t<X>;

template <class A, class X> auto pure(X &&x) {
  return A::pure(std::forward<X>(x));
}

template <class A, class X, class XYA>
auto bind(XYA &&xyA, const apply_t<A, X> xA) {
  return A::bind(std::forward<XYA>(xyA), xA);
}

template <class A, class X> auto &zero() {
  static apply_t<A, X> z = A::template zero<X>();
  return z;
}

template <class A, class X>
auto alt(const apply_t<A, X> &lhs, const apply_t<A, X> &rhs)
    -> decltype(A::alt(lhs, rhs)) {
  return A::alt(lhs, rhs);
}

//

template <class A, class X, class XY>
auto map(XY &&xy, const apply_t<A, X> &xA) {
  return bind<A>([=](auto &x) { return pure<A>(xy(x)); }, xA);
}

template <class A, class X0, class X1, class... Xs, class X0X1XsY>
auto map(X0X1XsY &&x0x1xsy, const apply_t<A, X0> &x0A,
         const apply_t<A, X1> &x1A, const apply_t<A, Xs> &... xsA) {
  return bind<A>(
      [=](const X0 &x0) {
        return map<A>([=](const X1 &x1,
                          const Xs &... xs) { return x0x1xsy(x0, x1, xs...); },
                      x1A, xsA...);
      },
      x0A);
}

//

template <class A, class X0> auto &alt(const apply_t<A, X0> &x0) { return x0; }

template <class A, class X0, class X1, class X2, class... Xs>
auto alt(const apply_t<A, X0> &x0, const apply_t<A, X1> &x1,
         const apply_t<A, X2> &x2, const apply_t<A, Xs> &... xs) {
  return alt<A>(x0, alt<A>(x1, alt<A>(x2, xs...)));
}
