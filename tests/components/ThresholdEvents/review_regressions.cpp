// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/ThresholdEvents.h>
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
using namespace superjupiter::threshold_events;

int main() {
  (void)close;

  ThresholdEvents alarm(Config(1, 2, 0, 0, 1000));
  CHECK(alarm.push(3, 0).state == State::Alarm);
  CHECK(alarm.push(NAN, 100).state == State::Unknown);
  CHECK(alarm.push(3, 99).status == Status::InvalidTime);
  CHECK(alarm.state() == State::Unknown);
  CHECK(alarm.push(0, 101).state == State::Normal);
  CHECK(alarm.push(NAN, 100).status == Status::InvalidTime &&
        alarm.state() == State::Normal);

  std::printf("PASS SuperJupiterThresholdEvents review: %u checks\n", checks);
}
