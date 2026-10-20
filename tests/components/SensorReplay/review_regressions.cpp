// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/testing/SensorReplay.h>
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
using namespace superjupiter::sensor_replay;

int main() {
  (void)close;

  Sample samples[] = {{1, 0, 0}, {2, 0, 1}, {3, 100, 0}};
  Fault fault[] = {{2, FaultKind::Replace, NAN}};
  SensorReplay replay(samples, 3, fault, 1);
  Output out = {};
  CHECK(replay.start(UINT32_MAX - 49) == Status::Ready);
  CHECK(replay.next(UINT32_MAX - 49, out) == Status::SampleReady &&
        out.channel == 0);
  CHECK(replay.next(UINT32_MAX - 49, out) == Status::SampleReady &&
        out.channel == 1);
  CHECK(replay.next(50, out) == Status::SampleReady && std::isnan(out.raw));
  CHECK(replay.next(50, out) == Status::Finished);
  CHECK(replay.next(49, out) == Status::InvalidTime);
  CHECK(replay.start(0) == Status::Ready && replay.consumed() == 0);

  std::printf("PASS SuperJupiterSensorReplay review: %u checks\n", checks);
}
