#pragma once

#include "detail/counted.hpp"
#include "detail/shared_sum.hpp"

#include <utility>

template <class V> struct shared_t {
  template <class... As, class = unless_contains_t<shared_t<V>, As...>>
  shared_t(As &&... vs) : m_body(new body(std::forward<As>(vs)...)) {}

  const V &deref() const { return m_body.deref().m_value; }

private:
  struct body : detail::counted_base {
    V m_value;
    template <class... As>
    body(As &&... vs) : m_value(std::forward<As>(vs)...) {}
  };

  detail::counted<body> m_body;
};

template <class... Es>
struct shared_t<sum_t<Es...>> : detail::shared_sum<Es...> {
  using detail::shared_sum<Es...>::shared_sum;

  template <class F> auto map(const F &fn) const {
    using result_t = shared_t<
        sum_t<the_t<tag_of_t<Es>, std::remove_cvref_t<std::invoke_result_t<
                                      F, const value_of_t<Es> &>>>...>>;
    result_t result;

    detail::apply_at<Es...>::apply(
        this->m_body.deref().m_index, &result, [&](auto *s) {
          using E = std::remove_cvref_t<decltype(*s)>;
          using T = tag_of_t<E>;

          result.m_body.m_ptr = new typename result_t::template alt<T>(fn(
              reinterpret_cast<
                  const typename detail::shared_sum<Es...>::template alt<T> *>(
                  &this->m_body.deref())
                  ->deref()));
        });

    return result;
  }

private:
  template <class V> friend struct shared_t;

  shared_t() {}
};

// it( ... )

template <class V> auto &it(const shared_t<V> &v) { return it(v.deref()); }

template <class... Es> auto &it(const shared_t<sum_t<Es...>> &s) { return s; }

// as<T>( ... )

template <class T, class V> auto as(const shared_t<V> &s) {
  return as<T>(it(s));
}

template <class T, class... Es> auto as(const shared_t<sum_t<Es...>> &s) {
  return s.template as<T>();
}

// get<T>( ... )

template <class T, class V> auto &get(const shared_t<V> &p) {
  return get<T>(it(p));
}
