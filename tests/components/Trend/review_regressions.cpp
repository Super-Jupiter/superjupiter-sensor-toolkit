// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/Trend.h>
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
using namespace superjupiter::trend;

int main() {
  (void)close;

  Trend<2> pair;
  CHECK(pair.push(1, 0) == Status::Ok);
  CHECK(pair.push(3, 1000) == Status::Ok);
  CHECK(pair.summary().slopePerSecond == 2);
  Trend<4> invalid(Config(5));
  CHECK(invalid.summary().status == Status::InvalidConfig);
  Trend<4> normal(Config(2, 0, 0x7fffffffUL));
  normal.push(1, UINT32_MAX);
  normal.push(2, 0);
  CHECK(normal.summary().status == Status::Ok);

  std::printf("PASS SuperJupiterTrend review: %u checks\n", checks);
}
