#include "list_strict.hpp"

#include "generics/pretty.hpp"

#include "util/test_runner.hpp"

#include "core/structures.hpp"

#include <iostream>

static auto list_strict_test = test_runner([]() {
  auto xs = list(3, 1, 4);
  auto ys = reverse(reverse(xs));
  auto zs = map<append>([](auto &x) { return 0.5 + x; }, ys);
  std::cerr << render(120, pretty(zs)) << std::endl;
});
