#include <SuperJupiter/SensorToolkit/SignalFilter.h>
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
using namespace superjupiter::signal_filter;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  SignalFilter f(Config(1000, 2000));
  CHECK(f.push(0, 0).status == Status::Seeded);
  CHECK(near(f.push(10, 1000).value, 10 * (1 - std::exp(-1.0))));
  CHECK(f.push(90, 1000).status == Status::InvalidTime);
  CHECK(f.push(NAN, 2000).status == Status::InvalidInput);
  CHECK(near(f.push(10, 2000).value, 10 * (1 - std::exp(-2.0))));
  CHECK(f.push(50, 4001).status == Status::GapReset);
  f.reset();
  CHECK(f.push(1, UINT32_MAX).status == Status::Seeded);
  CHECK(f.push(2, 0).status == Status::Filtered);
  CHECK(SignalFilter(Config(0)).push(1, 0).status == Status::InvalidConfig);

  SignalFilter constant;
  for (uint32_t i = 0; i < 1000; ++i)
    CHECK(near(constant.push(23, i * 17).value, 23));
  SignalFilter one(Config(1000, 5000)), two(Config(1000, 5000));
  one.push(0, 0);
  two.push(0, 0);
  one.push(10, 1000);
  two.push(10, 250);
  CHECK(near(one.push(10, 2000).value, two.push(10, 2000).value));
  CHECK(f.push(0, UINT32_MAX).status == Status::InvalidTime);

  std::printf("PASS SuperJupiterSignalFilter: %lu checks\n", checks);
}
