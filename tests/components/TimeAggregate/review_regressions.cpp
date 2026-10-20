// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/TimeAggregate.h>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
static unsigned checks = 0;
#define CHECK(x)                                                               \
  do {                                                                         \
    ++checks;                                                                  \
    if (!(x)) {                                                                \
      std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #x);         \
      std::abort();                                                            \
    }                                                                          \
  } while (false)
static bool close(double a, double b) {
  return std::fabs(a - b) <= 1e-12 * (1 + std::fabs(b));
}
using namespace superjupiter::time_aggregate;

int main() {
  (void)close;

  TimeAggregate aggregate(0x7fffffffUL);
  CHECK(aggregate.push(0, 0) == Status::Accepted);
  CHECK(aggregate.push(1, 0x7ffffffeUL) == Status::Accepted);
  CHECK(aggregate.push(2, 0xfffffffcUL) == Status::Completed);
  Summary done;
  CHECK(aggregate.takeCompleted(done));
  CHECK(done.mean == 0.5);
  CHECK(aggregate.push(3, 0x7ffffffaUL) == Status::Completed);
  CHECK(aggregate.takeCompleted(done));
  CHECK(done.mean == 2);
  TimeAggregate constant(1000);
  for (uint32_t i = 0; i < 100; ++i)
    CHECK(constant.push(DBL_MAX, i) == Status::Accepted);
  CHECK(constant.current().mean == DBL_MAX);

  std::printf("PASS SuperJupiterTimeAggregate review: %u checks\n", checks);
}
