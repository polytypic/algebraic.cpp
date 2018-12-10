#include "c_str_stream.hpp"

#include "util/test_runner.hpp"

static const char something[] = R"({"foo": [1, true, null]})";

static auto c_str_stream_test = test_runner([]() {
  auto s = c_str_stream_t::from(something);

  c_str_stream_t::pos(s);

  c_str_stream_t::read(s);
});
