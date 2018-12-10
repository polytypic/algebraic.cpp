#include "json.hpp"

#include "util/test_runner.hpp"

#include <iostream>

static auto json_test = test_runner(
    []() { std::cerr << "sizeof(json_t) == " << sizeof(json_t) << std::endl; });
