// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/SensorAgreement.h>
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
using namespace superjupiter::sensor_agreement;

int main() {
  (void)close;

  Sample samples[7];
  for (size_t i = 0; i < 7; ++i)
    samples[i] = Sample{42, 0};
  Result same = compare(samples, 7, Config(0, 0));
  CHECK(same.status == Status::Agree && same.mean == 42);
  samples[0].value = NAN;
  samples[1].ageMs = 1001;
  Result excluded = compare(samples, 7);
  CHECK(excluded.eligible == 5 && excluded.excluded == 2);
  CHECK(compare(0, 0).status == Status::Insufficient);
  CHECK(compare(0, 1).status == Status::InvalidConfig);

  std::printf("PASS SuperJupiterSensorAgreement review: %u checks\n", checks);
}
