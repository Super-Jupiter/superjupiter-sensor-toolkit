// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/ChannelAlign.h>
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
using namespace superjupiter::channel_align;

int main() {
  (void)close;

  ChannelAlign<1> invalid(0x80000000UL, 1);
  CHECK(invalid.push(0, 1, 0) != Status::Ok);
  ChannelAlign<1> invalidHold(1, 0x80000000UL);
  CHECK(invalidHold.push(0, 1, 0) != Status::Ok);
  ChannelAlign<1> align(1000, 100);
  align.push(0, -DBL_MAX, 0);
  align.push(0, DBL_MAX, 1000);
  CHECK(align.at(0, 500).status == Status::Interpolated &&
        align.at(0, 500).value == 0);
  CHECK(align.at(0, 1001).status == Status::Held);
  CHECK(align.at(0, 1101).status == Status::Stale);

  std::printf("PASS SuperJupiterChannelAlign review: %u checks\n", checks);
}
