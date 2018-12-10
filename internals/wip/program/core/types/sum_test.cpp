#include "sum.hpp"

#include "util/test_runner.hpp"

#include "generics/pretty.hpp"

#include <iostream>

struct empty_t {};

static auto option_test = test_runner([]() {
  std::cerr << sizeof(empty_t) << std::endl;
  std::cerr << sizeof(int) << std::endl;
  std::cerr << sizeof(the_t<L(Foo)>) << std::endl;
  std::cerr << sizeof(the_t<L(Foo), empty_t>) << std::endl;

  std::cerr << std::aligned_union<0, empty_t>::alignment_value << std::endl;
  std::cerr << sizeof(sum_t<the_t<L(Foo)>, the_t<L(Bar)>>) << std::endl;

  sum_t<the_t<L(A), int>, the_t<L(B), float>> s1 = the<L(A)>(69);

  sum_t<the_t<L(A), long>, the_t<L(C), float>, the_t<L(B), float>> s2 = s1;

  std::cerr << render(80, pretty(s2)) << std::endl;
});
