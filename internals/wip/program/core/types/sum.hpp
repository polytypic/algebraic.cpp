#pragma once

#include "the.hpp"

#include <new>
#include <type_traits>
#include <utility>

template <class... Es> struct sum_t {
  static_assert(!has_duplicates_t<seq_t<tag_of_t<Es>...>>::value,
                "Tags must be unique");

  ~sum_t() {
    detail::apply_at<Es...>::apply(m_index, &m_storage, [](auto *storage) {
      using E = std::remove_cvref_t<decltype(*storage)>;
      storage->~E();
    });
  }

  template <class... As, class = when_subset_of_t<seq_t<tag_of_t<Es>...>,
                                                  seq_t<tag_of_t<As>...>>>
  sum_t(const sum_t<As...> &that) {
    detail::apply_at<As...>::apply(
        that.m_index, &that.m_storage, [&](auto *from) {
          using T = tag_of_t<std::remove_cvref_t<decltype(*from)>>;
          using V = detail::value_of<T, Es...>;
          m_index = detail::index_of<T, Es...>::value;
          new (&m_storage) the_t<T, V>(*from);
        });
  }

  template <class... As, class = when_subset_of_t<seq_t<tag_of_t<Es>...>,
                                                  seq_t<tag_of_t<As>...>>>
  sum_t(sum_t<As...> &&that) {
    detail::apply_at<As...>::apply(
        that.m_index, &that.m_storage, [&](auto *from) {
          using T = tag_of_t<std::remove_cvref_t<decltype(*from)>>;
          using V = detail::value_of<T, Es...>;
          m_index = detail::index_of<T, Es...>::value;
          that.m_index = -1;
          new (&m_storage) the_t<T, V>(std::move(*from));
        });
  }

  template <class T, class I, class V = detail::value_of<T, Es...>>
  sum_t(the_t<T, I> &&v) : m_index(detail::index_of<T, Es...>::value) {
    new (&m_storage) the_t<T, V>(std::move(v));
  }

  template <class T, class I, class V = detail::value_of<T, Es...>>
  sum_t(const the_t<T, I> &v) : m_index(detail::index_of<T, Es...>::value) {
    new (&m_storage) the_t<T, V>(v);
  }

  template <class V, class T = detail::tag_of<V, Es...>>
  explicit sum_t(V &&v) : m_index(detail::index_of<T, Es...>::value) {
    new (&m_storage) the_t<T, V>(std::forward<V>(v));
  }

  template <class V, class T = detail::tag_of<V, Es...>>
  explicit sum_t(const V &v) : m_index(detail::index_of<T, Es...>::value) {
    new (&m_storage) the_t<T, V>(v);
  }

  sum_t(sum_t &&that) : m_index(that.m_index) {
    that.m_index = -1;
    detail::apply_at<Es...>::apply(m_index, &m_storage, [&](auto *storage) {
      using E = std::remove_cvref_t<decltype(*storage)>;
      new (storage) E(std::move(*reinterpret_cast<E *>(&that.m_storage)));
    });
  }

  sum_t(const sum_t &that) : m_index(that.m_index) {
    detail::apply_at<Es...>::apply(m_index, &m_storage, [&](auto *storage) {
      using E = std::remove_cvref_t<decltype(*storage)>;
      new (storage) E(*reinterpret_cast<const E *>(&that.m_storage));
    });
  }

  template <class F> auto map(const F &fn) const {
    using result_t =
        sum_t<the_t<tag_of_t<Es>, std::remove_cvref_t<std::invoke_result_t<
                                      F, const value_of_t<Es> &>>>...>;
    result_t result;
    result.m_index = m_index;
    detail::apply_at<Es...>::apply(m_index, &m_storage, [&](auto *storage) {
      using E = std::remove_cvref_t<decltype(*storage)>;
      using T = tag_of_t<E>;
      using V =
          std::remove_cvref_t<std::invoke_result_t<F, const value_of_t<E> &>>;
      new (&result.m_storage) the_t<T, V>(fn(it(*storage)));
    });
    return result;
  }

  template <class T> bool is() const {
    return m_index == detail::index_of<T, Es...>::value;
  }

  template <class T, class V = detail::value_of<T, Es...>> auto as() const {
    if (is<T>())
      return reinterpret_cast<const the_t<T, V> *>(&m_storage);
    return static_cast<const the_t<T, V> *>(nullptr);
  }

private:
  template <class... As> friend struct sum_t;

  sum_t() {}

  int m_index;
  std::aligned_union_t<0, Es...> m_storage;
};

// as<Tag>(sum>

template <class T, class... Es> auto as(const sum_t<Es...> &s) {
  return s.template as<T>();
}

template <class T, class S> auto as(const S *s) { return as<T>(it(s)); }

// get<Tag>(product)

template <class T, class S> auto &get(const sum_t<S> &p) {
  return get<T>(it(p));
}
