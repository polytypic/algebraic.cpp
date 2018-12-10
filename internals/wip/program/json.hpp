#pragma once

#include "types/key_value.hpp"
#include "types/list_strict.hpp"

#include "prettier/prettier.hpp"

#include <string>

using json_t = fix_t<tycon_t<
    seq_t<Rec>,
    sum_t<the_t<L(Object), list_t<key_value_t<std::string, Rec>>>,
          the_t<L(Array), list_t<Rec>>, the_t<L(String), shared_t<std::string>>,
          the_t<L(Number), double>, the_t<L(Boolean), bool>, none_t>>>;

extern doc_t pretty(const json_t &json);
