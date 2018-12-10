#include "list_lazy.hpp"

#include "generics/pretty.hpp"

#include "util/test_runner.hpp"

#include <iostream>

static auto list_lazy_test = test_runner([]() {
  list_t<int, lazy_t> fibs =
      cons(0, cons(1, list_t<int, lazy_t>(thunk([&]() {
                     return zip_with([&](auto x, auto y) { return x + y; },
                                     fibs, drop(1, fibs));
                   }))));

  std::cerr << render(120, pretty(take(4, drop(10, fibs)))) << std::endl;
});
