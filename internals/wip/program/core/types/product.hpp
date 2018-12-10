#pragma once

#include "the.hpp"

#include "util/meta.hpp"

#include <utility>

namespace detail {
template <class E, class... As>
using the_of_t = the_t<tag_of_t<E>, value_of<tag_of_t<E>, As...>>;
}

template <class E, class... Es>
struct product_t<E, Es...> : E, product_t<Es...> {
  static_assert(!has_duplicates_t<seq_t<tag_of_t<E>, tag_of_t<Es>...>>::value,
                "Tags must be unique");

  template <class... As, class = unless_contains_t<product_t, product_t<As...>>,
            class = when_subset_of_t<seq_t<tag_of_t<As>...>,
                                     seq_t<tag_of_t<E>, tag_of_t<Es>...>>>
  product_t(const product_t<As...> &that)
      : E(static_cast<const detail::the_of_t<E, As...> &>(that)),
        product_t<Es...>(that) {}

  template <class... As, class = unless_contains_t<product_t, product_t<As...>>,
            class = when_subset_of_t<seq_t<tag_of_t<As>...>,
                                     seq_t<tag_of_t<E>, tag_of_t<Es>...>>>
  product_t(product_t<As...> &&that)
      : E(std::move(static_cast<detail::the_of_t<E, As...> &>(that))),
        product_t<Es...>(std::move(that)) {}

  template <class A, class... As,
            class = unless_contains_t<product_t, A, As...>>
  explicit product_t(A &&v, As &&... vs)
      : E(std::forward<A>(v)), product_t<Es...>(std::forward<As>(vs)...) {}

  const E &head() const { return *this; }
  const product_t<Es...> &tail() const { return *this; }
};

template <> struct product_t<> {};

// product(the<T>(v)...)

template <class... Es> auto product(Es &&... es) {
  return product_t<Es...>(std::forward<Es>(es)...);
}
