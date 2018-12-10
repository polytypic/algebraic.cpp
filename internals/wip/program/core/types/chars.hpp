#pragma once

#include "fwd.hpp"
#include "util/meta.hpp"

// Note: In C++20 it should be possible to drastically simplify/eliminate this.

// chars_t<cs...>

template <char... cs> struct chars_t : is_type<chars_t<cs...>> {
  static constexpr const char value[sizeof...(cs) + 1] = {cs..., '\0'};
};

template <char... cs>
constexpr char const chars_t<cs...>::value[sizeof...(cs) + 1];

// chars_cat_t< ... > - Note: stops at first `chars_t<'\0'>`.

template <class... Ts> struct chars_cat_s;

template <class... Ts> using chars_cat_t = tpnm<chars_cat_s<Ts...>>;

template <char... cs0, char... cs1, class... Ts>
struct chars_cat_s<chars_t<cs0...>, chars_t<cs1...>, Ts...>
    : chars_cat_s<chars_t<cs0..., cs1...>, Ts...> {};

template <char... cs0, char... cs1, class... Ts>
struct chars_cat_s<chars_t<cs0...>, chars_t<'\0'>, chars_t<cs1...>, Ts...>
    : is_type<chars_t<cs0...>> {};

template <char... cs> struct chars_cat_s<chars_t<cs...>> : chars_t<cs...> {};

namespace detail {
template <int N, int M> constexpr char at(const char (&c)[M]) noexcept {
  return c[N < M ? N : M - 1];
}

template <char... cs> using make_chars = chars_cat_t<chars_t<cs>...>;
} // namespace detail

// L(cs...)

#define L(x)                                                                   \
  detail::make_chars<                                                          \
      detail::at<0x00>(#x), detail::at<0x01>(#x), detail::at<0x02>(#x),        \
      detail::at<0x03>(#x), detail::at<0x04>(#x), detail::at<0x05>(#x),        \
      detail::at<0x06>(#x), detail::at<0x07>(#x), detail::at<0x08>(#x),        \
      detail::at<0x09>(#x), detail::at<0x0a>(#x), detail::at<0x0b>(#x),        \
      detail::at<0x0c>(#x), detail::at<0x0d>(#x), detail::at<0x0e>(#x),        \
      detail::at<0x0f>(#x), detail::at<0x10>(#x), detail::at<0x11>(#x),        \
      detail::at<0x12>(#x), detail::at<0x13>(#x), detail::at<0x14>(#x),        \
      detail::at<0x15>(#x), detail::at<0x16>(#x), detail::at<0x17>(#x),        \
      detail::at<0x18>(#x), detail::at<0x19>(#x), detail::at<0x1a>(#x),        \
      detail::at<0x1b>(#x), detail::at<0x1c>(#x), detail::at<0x1d>(#x),        \
      detail::at<0x1e>(#x), detail::at<0x1f>(#x)>

// name_of_t<T>

template <class T> struct name_of_s;

template <class T> using name_of_t = tpnm<name_of_s<T>>;

//

template <char... cs> struct name_of_s<chars_t<cs...>> : chars_t<cs...> {};

template <> struct name_of_s<void> : L(void) {};

template <> struct name_of_s<char> : L(char) {};
template <> struct name_of_s<short> : L(short) {};
template <> struct name_of_s<int> : L(int) {};
template <> struct name_of_s<long> : L(long) {};
template <> struct name_of_s<long long> : L(long long) {};

template <> struct name_of_s<unsigned char> : L(unsigned char) {};
template <> struct name_of_s<unsigned short> : L(unsigned short) {};
template <> struct name_of_s<unsigned int> : L(unsigned int) {};
template <> struct name_of_s<unsigned long> : L(unsigned long) {};
template <> struct name_of_s<unsigned long long> : L(unsigned long long) {};

template <> struct name_of_s<float> : L(float) {};
template <> struct name_of_s<double> : L(double) {};
template <> struct name_of_s<long double> : L(long double) {};
