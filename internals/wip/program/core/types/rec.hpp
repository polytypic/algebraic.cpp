#pragma once

#include "chars.hpp"

#include "util/meta.hpp"
#include "util/tycon.hpp"

#include <utility>

template <template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
struct rec_t {
  using body_t = TyCon<rec_t<TyCon, TyArgs...>, TyArgs...>;

  template <class... As, class = unless_contains_t<rec_t, As...>>
  rec_t(As &&... vs) : m_body(std::forward<As>(vs)...) {}

  const body_t &unwrap() const { return m_body; }

private:
  body_t m_body;
};

template <class T> using body_t = typename T::body_t;

// unwrap( ... )

template <class X> auto &unwrap(const X &x) { return x; }

template <template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
auto &unwrap(const rec_t<TyCon, TyArgs...> &r) {
  return r.unwrap();
}

// as<T>( ... )

template <class T, template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
auto as(const rec_t<TyCon, TyArgs...> &r) {
  return as<T>(unwrap(r));
}

// get<T>( ... )

template <class T, template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
auto &get(const rec_t<TyCon, TyArgs...> &r) {
  return get<T>(unwrap(r));
}

// fix_t

using Rec = L(Rec);
using Arg = L(Arg);

template <class TyCon, class... As>
using fix_t = rec_t<TyCon::template F, As...>;
