#include "option.hpp"

#include "core/structures.hpp"

#include "util/test_runner.hpp"

#include "generics/pretty.hpp"

#include <iostream>
#include <string>

static option_t<int> make(bool whether, int value) {
  if (whether)
    return some(value);
  return none;
}

static auto option_test = test_runner([]() {
  option_t<std::string> os = some(std::string("1"));

  option_t<int> oi = make(true, 1);

  option_t<int *> op = some(static_cast<int *>(nullptr));

  option_t<float> of = none;

  option_t<product_t<the_t<int>, the_t<float>>> oifp =
      some(product(the(1.0f), the(1)));

  std::cerr << render(80, pretty(oifp)) << std::endl;

  option_t<sum_t<the_t<int>, the_t<float>>> oifs = some(42);

  std::cerr << render(80, pretty(oifs)) << std::endl;

  auto xx =
      map<option>([](auto &x) { return product(the<L(X)>(x), the<L(Y)>(x)); },
                  option_t<int>(1));

  std::cerr << render(80, pretty(xx)) << std::endl;
});
