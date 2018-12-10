#include "trace.hpp"

#include <cstdio>
#include <cstdlib>

extern void trace(const char *file, const char *function, int line) {
  fprintf(stderr, "%s(%s@%d)\n", file, function, line);
}
