#include "panic.hpp"

#include <cstdio>
#include <cstdlib>

[[noreturn]] extern void panic(const char *message, const char *file,
                               const char *function, int line) {
  fprintf(stderr, "%s(%s@%d): %s\n", file, function, line, message);
  exit(1);
}
