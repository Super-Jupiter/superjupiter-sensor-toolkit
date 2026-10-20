// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/Integration.h>
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
using namespace superjupiter::integration;

int main() {
  (void)close;

  Integration integral;
  integral.push(1, 0);
  CHECK(integral.push(NAN, 100).status == Status::InvalidInput);
  CHECK(integral.push(2, 99).status == Status::InvalidTime);
  CHECK(integral.push(2, 101).status == Status::Seeded);
  CHECK(integral.push(2, 201).valueSeconds == 0.2);
  CHECK(integral.push(NAN, 200).status == Status::InvalidTime);
  CHECK(integral.push(2, 301).valueSeconds == 0.4);
  CHECK(Integration(1000, static_cast<Method>(99)).push(1, 0).status ==
        Status::InvalidConfig);
  const double tiny = std::numeric_limits<double>::denorm_min();
  Integration verySmall;
  verySmall.push(tiny, 0);
  CHECK(verySmall.push(tiny, 1000).valueSeconds == tiny);
  Integration smallRamp;
  smallRamp.push(tiny, 0);
  CHECK(smallRamp.push(5 * tiny, 1000).valueSeconds == 3 * tiny);
  Integration veryLarge;
  veryLarge.push(DBL_MAX, 0);
  CHECK(veryLarge.push(DBL_MAX, 1000).valueSeconds == DBL_MAX);

  std::printf("PASS SuperJupiterIntegration review: %u checks\n", checks);
}
