// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/Calibration.h>
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
using namespace superjupiter::calibration;

int main() {
  (void)close;

  Point points[] = {{0, -DBL_MAX}, {1, DBL_MAX}};
  Calibration extreme(points, 2);
  CHECK(extreme.evaluate(0).valid() && extreme.evaluate(0).value == -DBL_MAX);
  CHECK(extreme.evaluate(1).valid() && extreme.evaluate(1).value == DBL_MAX);
  CHECK(extreme.evaluate(0.5).valid() && extreme.evaluate(0.5).value == 0);
  Point wide[] = {{-DBL_MAX, 0}, {DBL_MAX, 1}};
  CHECK(Calibration(wide, 2).evaluate(0).value == 0.5);
  CHECK(!Calibration(wide, 2, 0, static_cast<Bounds>(99)).valid());
  Point zeros[] = {{0, 0.0}, {1, -0.0}};
  CHECK(std::signbit(Calibration(zeros, 2).evaluate(1).value));
  CHECK(!std::signbit(Calibration(zeros, 2).evaluate(0).value));

  std::printf("PASS SuperJupiterCalibration review: %u checks\n", checks);
}
