#pragma once

#include "types/option.hpp"

#include <cstddef>

struct c_str_stream_t {
  using pos_t = std::size_t;
  using elem_t = char;

  using stream_t =
      product_t<the_t<L(Pos), pos_t>, the_t<L(Begin), const elem_t *>>;

  using read_t =
      option_t<product_t<the_t<L(Head), elem_t>, the_t<L(Tail), stream_t>>>;

  static stream_t from(const elem_t *b) { return stream(0, b); }

  static pos_t pos(const stream_t &s) { return get<L(Pos)>(s); }

  static read_t read(const stream_t &s) {
    elem_t result = *(begin(s) + pos(s));
    if (result)
      return some(product(the<L(Head)>(result),
                          the<L(Tail)>(stream(pos(s) + 1, begin(s)))));
    return none;
  }

private:
  static stream_t stream(pos_t p, const elem_t *b) {
    return product(the<L(Pos)>(p), the<L(Begin)>(b));
  }

  static const elem_t *begin(const stream_t &s) { return get<L(Begin)>(s); }
};
