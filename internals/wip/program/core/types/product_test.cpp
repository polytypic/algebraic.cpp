#include "product.hpp"

#include "util/test_runner.hpp"

#include "generics/pretty.hpp"

#include <iostream>
#include <string>

static auto product_test = test_runner([]() {
  {
    product_t<the_t<L(B), double>, the_t<L(A), long>> ab =
        product(the<L(A)>(101), the<L(B)>(3.14f));

    std::cerr << render(80, pretty(ab)) << std::endl;
  }

  {
    product_t<the_t<L(Key), std::string>, the_t<L(Value), int>> p =
        product(the<L(Key)>(std::string("abba")), the<L(Value)>(101));

    product_t<the_t<L(Key), std::string>, the_t<L(Value), int>> q(std::move(p));
  }
});
