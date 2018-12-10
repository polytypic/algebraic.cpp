#pragma once

#include "core/types.hpp"

template <class V> using some_t = the_t<L(Some), V>;

template <class V> some_t<V> some(const V &v) { return the<L(Some)>(v); }
