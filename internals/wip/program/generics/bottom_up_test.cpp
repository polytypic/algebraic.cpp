#include "bottom_up.hpp"

#include "generics/pretty.hpp"

#include "util/test_runner.hpp"

#include <iostream>

static auto bottom_up_test = test_runner([]() {
  {
    auto p0 = product(the(1), the(1.5f));
    auto p1 = bottom_up_at<int>([](const int &x) -> int { return x + 1; }, p0);
    std::cerr << render(120, pretty(p1)) << std::endl;
  }

  {
    sum_t<the_t<L(A), int>, the_t<L(B), float>> s0 = the<L(A)>(1);
    auto s1 = bottom_up_at<int>([](const int &x) { return list(x); }, s0);
    std::cerr << render(120, pretty(s1)) << std::endl;
  }

  {
    shared_t<sum_t<the_t<L(A), int>, the_t<L(B), float>>> s0 = the<L(A)>(1);
    auto s1 = bottom_up_at<int>([](const int &x) { return list(x); }, s0);
    std::cerr << render(120, pretty(s1)) << std::endl;
  }
  /*
    {
      auto xs = list(3, 1, 4);
      auto ys = bottom_up(
        [](const list_t<int> &xs) { return cons(0, xs); },
        xs
      );
    }
  */
});
