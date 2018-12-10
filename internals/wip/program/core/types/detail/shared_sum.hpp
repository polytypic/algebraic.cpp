#pragma once

#include "core/types/sum.hpp"
#include "counted.hpp"

namespace detail {
struct alt_base : counted_base {
  int m_index;
  alt_base(int index) : m_index(index) {}
};

template <class... Es> struct shared_sum {

  template <class T, class I, int index = index_of<T, Es...>::value>
  shared_sum(the_t<T, I> &&v) : m_body(new alt<T>(std::move(v))) {}

  template <class T, class I, int index = index_of<T, Es...>::value>
  shared_sum(const the_t<T, I> &v) : m_body(new alt<T>(v)) {}

  template <class V, class T = tag_of<V, Es...>>
  explicit shared_sum(V &&v) : m_body(new alt<T>(std::forward<V>(v))) {}

  template <class V, class T = tag_of<V, Es...>>
  explicit shared_sum(const V &v) : m_body(new alt<T>(v)) {}

  template <class T> bool is() const {
    return m_body.deref().m_index == index_of<T, Es...>::value;
  }

  template <class T, class V = value_of<T, Es...>>
  const the_t<T, V> *as() const {
    if (is<T>())
      return reinterpret_cast<const alt<T> *>(&m_body.deref());
    return static_cast<const the_t<T, V> *>(nullptr);
  }

protected:
  shared_sum() : m_body(nullptr) {}

  counted<alt_base> m_body;

  template <class T> struct alt : alt_base, the_t<T, value_of<T, Es...>> {
    template <class... As>
    alt(As &&... vs)
        : alt_base(index_of<T, Es...>::value), the_t<T, value_of<T, Es...>>(
                                                   std::forward<As>(vs)...) {}
  };
};

} // namespace detail
