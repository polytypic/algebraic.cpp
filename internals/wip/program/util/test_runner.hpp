#pragma once

struct test_runner_t {};

#ifdef NDEBUG

template <class Action> test_runner_t test_runner(Action &&) { return {}; }

#else

template <class Action> test_runner_t test_runner(Action &&action) {
  action();
  return {};
}

#endif
