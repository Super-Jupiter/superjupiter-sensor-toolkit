// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/SensorQuality.h>
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
using namespace superjupiter::sensor_quality;

int main() {
  (void)close;

  Config config;
  config.rangeEnabled = true;
  config.minimum = -10;
  config.maximum = 100;
  config.staleAfterMs = 100;
  SensorQuality sensor(config);
  CHECK(sensor.observe(20, UINT32_MAX - 49).healthy());
  CHECK(!sensor.observe(21, 50).hasRate);
  CHECK(sensor.inspect(150).has(Stale));
  CHECK(sensor.observe(NAN, 150).has(InvalidNumber));
  CHECK(sensor.observe(25, 149).has(InvalidTimestamp));
  CHECK(sensor.observe(25, 151).hasReading);
  CHECK(!sensor.observe(25, 151).healthy());
  CHECK(std::strcmp(flagName(static_cast<Flag>(1U << 9)), "numeric_error") ==
        0);

  std::printf("PASS SuperJupiterSensorQuality review: %u checks\n", checks);
}
