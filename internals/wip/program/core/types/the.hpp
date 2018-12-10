#pragma once

#include "chars.hpp"

#include "util/meta.hpp"
#include "util/panic.hpp"

#include "detail/value.hpp"

#include "polyfill_v1/type_traits.hpp"
#include <utility>

template <class T, class V> struct the_t : detail::value<V> {
  using tag_t = T;
  using value_t = V;

  template <class... As, class = unless_contains_t<the_t, As...>>
  explicit the_t(As &&... vs) : detail::value<V>(std::forward<As>(vs)...) {}

  template <class W>
  explicit the_t(the_t<T, W> &&that)
      : detail::value<V>(std::move(that.deref())) {}

  template <class W>
  explicit the_t(const the_t<T, W> &that) : detail::value<V>(that.deref()) {}

private:
  template <class U, class W> friend struct the_t;
};

template <class X> using tag_of_t = typename X::tag_t;

template <class X> using value_of_t = typename X::value_t;

// the<T>(value)

template <class T, class V> auto the(V &&v) {
  return the_t<T, std::remove_cvref_t<V>>(std::forward<V>(v));
}

template <class V> auto the(V &&v) {
  return the_t<std::remove_cvref_t<V>>(std::forward<V>(v));
}

// it( ... )

template <class V> const V &it(const V &v) { return v; }

template <class T, class V> auto &it(const the_t<T, V> &v) {
  return it(v.deref());
}

template <class T, class V> auto &it(const the_t<T, V> *p) {
  if (!p)
    PANIC(name_of_t<T>::value);
  return it(p->deref());
}

// get<Tag>(product)

template <class T, class V> auto &get(const the_t<T, V> &v) {
  return v.deref();
}

template <class T, class U, class V> auto &get(const the_t<U, V> *p) {
  return get<T>(it(p));
}

namespace detail {

// index_of

template <class T, class... Es> struct index_of {};

template <class T, class V, class... Es>
struct index_of<T, the_t<T, V>, Es...> : is_value<int, 0> {};

template <class T, class E0, class... Es>
struct index_of<T, E0, Es...> : is_value<int, 1 + index_of<T, Es...>::value> {};

// tag_of

template <class V, class... Es> struct tag_of_s {};
template <class... As> using tag_of = tpnm<tag_of_s<As...>>;

template <class V, class T, class... Es>
struct tag_of_s<V, the_t<T, V>, Es...> : is_type<T> {};

template <class V, class E0, class... Es>
struct tag_of_s<V, E0, Es...> : tag_of_s<V, Es...> {};

// apply_at

template <class... Es> struct apply_at {};

template <class E0, class... Es> struct apply_at<E0, Es...> {
  template <class Fn>
  static inline void apply(int i, void *storage, const Fn &fn) {
    if (i == 0)
      fn(static_cast<E0 *>(storage));
    else
      apply_at<Es...>::apply(i - 1, storage, fn);
  }

  template <class Fn>
  static inline void apply(int i, const void *storage, const Fn &fn) {
    if (i == 0)
      fn(static_cast<const E0 *>(storage));
    else
      apply_at<Es...>::apply(i - 1, storage, fn);
  }
};

template <> struct apply_at<> {
  template <class Fn> static inline void apply(int, void *, const Fn &) {}

  template <class Fn> static inline void apply(int, const void *, const Fn &) {}
};

// value_of

template <class T, class... Es> struct value_of_s {};
template <class... As> using value_of = tpnm<value_of_s<As...>>;

template <class T, class V, class... Es>
struct value_of_s<T, the_t<T, V>, Es...> : is_type<V> {};

template <class T, class E0, class... Es>
struct value_of_s<T, E0, Es...> : value_of_s<T, Es...> {};
} // namespace detail
