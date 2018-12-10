#pragma once

#include "core/types.hpp"

template <class L, class R = L>
using lhs_rhs_t = product_t<the_t<L(Lhs), L>, the_t<L(Rhs), R>>;

template <class LR> auto &lhs(const LR &lr) { return get<L(Lhs)>(lr); }

template <class LR> auto &rhs(const LR &lr) { return get<L(Rhs)>(lr); }
