#pragma once

#include "mut.hpp"
#include "rec.hpp"
#include "shared.hpp"
#include "sum.hpp"
#include "the.hpp"

#include <functional>

template <class V> struct lazy_t {
  template <class Th>
  lazy_t(the_t<L(Thunk), Th> &&th) : m_state(std::move(th)) {}

  template <class... As, class = unless_contains_t<lazy_t<V>, As...>>
  lazy_t(As &&... vs) : m_state(the_t<L(Value), V>(std::forward<As>(vs)...)) {}

  operator const V &() const { return force(); }

  const V &force() const {
    if (auto delay = as<L(Thunk)>(m_state))
      const_cast<inner_t &>(m_state.deref()) = the<L(Value)>(it(delay)());
    return it(as<L(Value)>(m_state));
  }

private:
  using inner_t =
      mut_t<sum_t<the_t<L(Value), V>, the_t<L(Thunk), std::function<V()>>>>;
  shared_t<inner_t> m_state;
};

// lazy_t<T> v(thunk([]() { ... }))

template <class Th> auto thunk(Th &&th) {
  return the<L(Thunk)>([th = std::forward<Th>(th)]() { return unwrap(th()); });
}

// it( ... )

template <class V> auto &it(const lazy_t<V> &v) { return it(v.force()); }

// as<T>( ... )

template <class T, class V> auto as(const lazy_t<V> &s) { return as<T>(it(s)); }

// get<T>( ... )

template <class T, class V> auto &get(const lazy_t<V> &p) {
  return get<T>(it(p));
}
