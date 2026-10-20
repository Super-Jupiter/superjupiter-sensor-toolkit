#include <SuperJupiter/SensorToolkit/Trend.h>
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
using namespace superjupiter::trend;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  Trend<4> t;
  CHECK(t.summary().status == Status::Insufficient);
  CHECK(t.push(2, 0) == Status::Ok);
  t.push(4, 1000);
  t.push(8, 3000);
  Result r = t.summary();
  CHECK(r.status == Status::Ok);
  CHECK(near(r.slopePerSecond, 2));
  CHECK(near(r.intercept, 2));
  CHECK(r.spanMs == 3000);
  CHECK(t.push(NAN, 4000) == Status::InvalidInput);
  CHECK(t.push(10, 3000) == Status::InvalidTime);
  t.push(10, 4000);
  t.push(12, 5000);
  CHECK(t.summary().count == 4);
  CHECK(near(t.summary().intercept, 4));
  t.push(0, 20000);
  CHECK(t.summary().status == Status::Gap);
  Trend<2> small(Config(2, 1, 100));
  small.push(1, UINT32_MAX);
  small.push(2, 0);
  CHECK(near(small.summary().slopePerSecond, 1000));

  Trend<16> linear(Config(3, 10, 1000));
  for (uint32_t i = 0; i < 1000; ++i) {
    CHECK(linear.push(3 + 0.2 * i, i * 100) == Status::Ok);
    if (i >= 2) {
      Result v = linear.summary();
      CHECK(v.status == Status::Ok);
      CHECK(near(v.slopePerSecond, 2));
    }
  }
  Trend<3> invalidConfig(Config(4));
  CHECK(invalidConfig.push(1, 0) == Status::InvalidConfig);

  std::printf("PASS SuperJupiterTrend: %lu checks\n", checks);
}
