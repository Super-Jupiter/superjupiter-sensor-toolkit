// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/PulseMetrics.h>
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
using namespace superjupiter::pulse_metrics;

int main() {
  (void)close;

  PulseMetrics meter(Config(10, 1000, 0.1));
  CHECK(meter.pulse(UINT32_MAX - 99).status == Status::First);
  CHECK(close(meter.pulse(0).ratePerSecond, 1000));
  CHECK(meter.pulse(5).status == Status::Bounce);
  CHECK(meter.poll(1001).status == Status::TimedOut);
  CHECK(meter.pulse(2000).status == Status::NoRate);
  CHECK(meter.pulse(2010).count == 4);

  std::printf("PASS SuperJupiterPulseMetrics review: %u checks\n", checks);
}
