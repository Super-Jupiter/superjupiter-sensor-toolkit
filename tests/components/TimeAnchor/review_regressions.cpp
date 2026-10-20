// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/TimeAnchor.h>
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
using namespace superjupiter::time_anchor;

int main() {
  (void)close;

  TimeAnchor clock(Config(0x7fffffffUL, UINT32_MAX));
  CHECK(clock.setAnchor(UINT32_MAX, 1000, UINT32_MAX) == Status::Ok);
  Result converted = clock.convert(0x7ffffffeUL);
  CHECK(converted.status == Status::Ok && converted.utcMs == 2147484647LL);
  CHECK(converted.uncertaintyMs >= UINT32_MAX);
  CHECK(clock.convert(0x7fffffffUL).status == Status::OutsideHorizon);
  clock.clear();
  CHECK(clock.convert(0).status == Status::NoAnchor);

  std::printf("PASS SuperJupiterTimeAnchor review: %u checks\n", checks);
}
