#include "lazy.hpp"

#include "util/test_runner.hpp"

#include <iostream>

static auto lazy_test = test_runner([]() {
  lazy_t<int> x(thunk([]() {
    std::cerr << "computing x" << std::endl;
    return 101;
  }));

  lazy_t<int> y(x);

  std::cerr << it(x) << std::endl;

  std::cerr << it(y) << std::endl;
});
