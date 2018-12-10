#pragma once

#include "core/types/fwd.hpp"

#include "util/meta.hpp"

#include <type_traits>
#include <utility>

namespace detail {
template <class V, bool is_empty = std::is_empty<V>::value> struct value {
  template <class... As, class = unless_contains_t<value<V>, As...>>
  explicit value(As &&... vs) : m_value(std::forward<As>(vs)...) {}

  const V &deref() const { return m_value; }

protected:
  template <class U, class W> friend struct the;
  V &deref() { return m_value; }
  V m_value;
};

template <class V, std::size_t n> struct value<V[n], false> {
  using VA = V[n];

  template <class... As, class = unless_contains_t<value<VA>, As...>>
  explicit value(As &&... vs) : m_value{std::forward<As>(vs)...} {}

  const VA &deref() const { return m_value; }

protected:
  template <class U, class W> friend struct the;
  VA &deref() { return m_value; }
  VA m_value;
};

template <class V> struct value<V, true> : private V {
  template <class... As, class = unless_contains_t<value<V>, As...>>
  explicit value(As &&... vs) : V(vs...) {}

  const V &deref() const { return *this; }

protected:
  template <class U, class W> friend struct the;
  V &deref() { return *this; }
};
} // namespace detail
