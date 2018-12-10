#pragma once

#include "util/meta.hpp"

#include <utility>

template <class V> struct mut_t {
  template <class... As, class = unless_contains_t<mut_t<V>, As...>>
  mut_t(As &&... vs) : m_value(std::forward<As>(vs)...) {}

  mut_t(mut_t &&that) : m_value(std::move(that.m_value)) {}

  mut_t(const mut_t &that) : m_value(that.m_value) {}

  const V &deref() const { return m_value; }

  V &deref() { return m_value; }

  void swap(mut_t &that) {
    V tmp(std::move(m_value));
    new (&m_value) V(std::move(that.m_value));
    new (&that.m_value) V(std::move(tmp));
  }

  mut_t &operator=(mut_t &&that) {
    V destroyer(std::move(m_value));
    new (&m_value) V(std::move(that.m_value));
    return *this;
  }

  mut_t &operator=(const mut_t &that) {
    mut_t tmp(that);
    swap(tmp);
    return *this;
  }

private:
  V m_value;
};

// mut( v )

template <class V> auto mut(V &&v) {
  return mut_t<std::remove_cvref_t<V>>(std::forward<V>(v));
}

// it( ... )

template <class V> auto &it(const mut_t<V> &v) { return it(v.deref()); }

// as<T>( ... )

template <class T, class V> auto as(const mut_t<V> &s) { return as<T>(it(s)); }

// get<T>( ... )

template <class T, class V> auto &get(const mut_t<V> &p) {
  return get<T>(it(p));
}
