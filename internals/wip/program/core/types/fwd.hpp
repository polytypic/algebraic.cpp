#pragma once

#include "util/trace.hpp"

template <char... cs> struct chars_t;

template <class T, class V = T> struct the_t;

template <class... Es> struct product_t;

template <class... Es> struct sum_t;

template <class V> struct shared_t;

template <class V> struct lazy_t;

template <template <class Result, class... TyParams> class TyCon,
          class... TyArgs>
struct rec_t;

template <class V> struct mut_t;
