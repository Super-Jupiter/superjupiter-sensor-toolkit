// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/SignalFilter.h>
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
using namespace superjupiter::signal_filter;

int main() {
  (void)close;

  SignalFilter filter(Config(1e12, 1000));
  filter.push(0, 0);
  Result result = filter.push(1, 1);
  CHECK(result.status == Status::Filtered && result.value > 0 &&
        close(result.value, -std::expm1(-1e-12)));
  CHECK(filter.push(NAN, 2).status == Status::InvalidInput);
  CHECK(filter.push(1, 1).status == Status::InvalidTime);
  SignalFilter constant;
  for (uint32_t i = 0; i < 1000; ++i)
    CHECK(close(constant.push(1e200, i + 1).value, 1e200));

  std::printf("PASS SuperJupiterSignalFilter review: %u checks\n", checks);
}
