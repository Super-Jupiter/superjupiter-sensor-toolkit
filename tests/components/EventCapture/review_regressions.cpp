// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/EventCapture.h>
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
using namespace superjupiter::event_capture;

int main() {
  (void)close;

  EventCapture<3, 2> capture;
  for (uint32_t i = 0; i < 100; ++i)
    capture.push(i, i);
  CHECK(capture.trigger() == Status::Capturing);
  capture.push(NAN, 100);
  CHECK(capture.push(101, 101) == Status::Complete);
  Sample output;
  for (size_t i = 0; i < 3; ++i)
    CHECK(capture.get(i, output) && output.raw == static_cast<double>(97 + i));
  CHECK(capture.get(3, output) && std::isnan(output.raw));
  CHECK(capture.size() == 5);
  capture.reset();
  CHECK(capture.size() == 0);

  std::printf("PASS SuperJupiterEventCapture review: %u checks\n", checks);
}
