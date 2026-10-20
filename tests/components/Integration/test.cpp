#include <SuperJupiter/SensorToolkit/Integration.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <new>
static unsigned long checks = 0;
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    ++checks;                                                                                      \
    if (!(x)) {                                                                                    \
      std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x);                                    \
      std::abort();                                                                                \
    }                                                                                              \
  } while (false)
static bool near(double a, double b) { return std::fabs(a - b) <= 1e-9 * (1 + std::fabs(b)); }
using namespace superjupiter::integration;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  Integration a(2000);
  a.push(0, 0);
  CHECK(a.push(10, 1000).valueSeconds == 5);
  CHECK(a.push(10, 2000).valueSeconds == 15);
  Result r = a.push(10, 5000);
  CHECK(r.status == Status::GapSkipped && r.skippedMs == 3000 && r.coveredMs == 2000);
  CHECK(a.push(NAN, 6000).status == Status::InvalidInput);
  CHECK(a.push(10, 7000).status == Status::Seeded);
  CHECK(a.push(10, 8000).valueSeconds == 25);
  CHECK(a.push(0, 8000).status == Status::InvalidTime);
  a.reset();
  CHECK(a.summary().valueSeconds == 0);
  Integration p(1000, Method::Previous);
  p.push(2, 0);
  CHECK(p.push(100, 1000).valueSeconds == 2);
  Integration w;
  w.push(-2, UINT32_MAX - 499);
  CHECK(w.push(-2, 500).valueSeconds == -2);

  Integration ramp(1000);
  for (uint32_t i = 0; i <= 1000; ++i)
    ramp.push(2 + 3 * (i / 10.0), i * 100);
  CHECK(near(ramp.summary().valueSeconds, 15200));
  CHECK(ramp.summary().coveredMs == 100000);
  Integration huge(10000);
  huge.push(1e308, 0);
  CHECK(huge.push(1e308, 10000).status == Status::NumericError);
  CHECK(huge.summary().coveredMs == 0);

  std::printf("PASS SuperJupiterIntegration: %lu checks\n", checks);
}
