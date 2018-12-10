#pragma once

#include "prettier/prettier.hpp"

#include <string>
#include <type_traits>

// primitives

inline doc_t pretty(char c) { return squote() + txt(c) + squote(); } // TODO
inline doc_t pretty(const std::string &s) {
  return dquote() + txt(s) + dquote();
} // TODO
inline doc_t pretty(const char *s) {
  return dquote() + txt(s) + dquote();
} // TODO

template <class V, class = std::enable_if_t<std::is_arithmetic<V>::value>>
doc_t pretty(V v) {
  return txt(std::to_string(v));
}

// indirections

template <class V> doc_t pretty(const shared_t<V> &v) { return pretty(it(v)); }

template <class V> doc_t pretty(const lazy_t<V> &v) { return pretty(it(v)); }

template <template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
doc_t pretty(const rec_t<TyCon, TyArgs...> &v) {
  return pretty(unwrap(v));
}

// chars

template <char... cs> doc_t pretty(const chars_t<cs...> &) {
  return txt(name_of_t<chars_t<cs...>>::value);
}

// products

namespace detail {
template <class T, class V> doc_t pretty_elem(const the_t<T, V> &v) {
  return gnest(2, txt(name_of_t<T>::value) + colonline() + pretty(it(v)));
}

inline doc_t pretty_product(const product_t<> &) { return empty(); }

template <class E0, class... Es>
doc_t pretty_product(const product_t<E0, Es...> &v) {
  return commaline() + pretty_elem(v.head()) + pretty_product(v.tail());
}
} // namespace detail

inline doc_t pretty(const product_t<> &) { return txt("{}"); }

template <class E0, class... Es> doc_t pretty(const product_t<E0, Es...> &v) {
  return group(nest(2, lbrace() + linebreak() + detail::pretty_elem(v.head()) +
                           detail::pretty_product(v.tail())) +
               linebreak() + rbrace());
}

// sums

namespace detail {
template <class T, class V> doc_t pretty_case(const the_t<T, V> &v) {
  return group(txt(name_of_t<T>::value) + lparen() + pretty(it(v)) + rparen());
}

template <class T> doc_t pretty_case(const the_t<T> &v) {
  return pretty(it(v));
}

template <class... Es> struct pretty_sum;

template <> struct pretty_sum<> {
  template <class S> static doc_t doit(const S &) { PANIC("Impossible"); }
};

template <class T, class V, class... Es> struct pretty_sum<the_t<T, V>, Es...> {
  template <class S> static doc_t doit(const S &s) {
    if (auto v = as<T>(s))
      return pretty_case(*v);
    return pretty_sum<Es...>::doit(s);
  }
};
} // namespace detail

template <class... Es> doc_t pretty(const sum_t<Es...> &v) {
  return detail::pretty_sum<Es...>::doit(v);
}

template <class... Es> doc_t pretty(const shared_t<sum_t<Es...>> &v) {
  return detail::pretty_sum<Es...>::doit(v);
}
