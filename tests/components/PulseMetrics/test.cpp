#include <SuperJupiter/SensorToolkit/PulseMetrics.h>
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
using namespace superjupiter::pulse_metrics;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  PulseMetrics p(Config(100, 10000, 2));
  CHECK(p.poll(0).status == Status::NoRate);
  CHECK(p.pulse(UINT32_MAX - 499).status == Status::First);
  CHECK(p.pulse(500).ratePerSecond == 2000);
  CHECK(p.pulse(550).status == Status::Bounce);
  CHECK(p.poll(10501).status == Status::TimedOut);
  CHECK(p.pulse(20000).status == Status::NoRate);
  CHECK(p.pulse(21000).ratePerSecond == 2000);
  CHECK(p.pulse(21000).status == Status::InvalidTime);
  CHECK(p.poll(21000).count == 4);
  CHECK(!PulseMetrics(Config(0)).valid());

  PulseMetrics stream(Config(1, 10000, 0.25));
  for (uint32_t i = 0; i < 1000; ++i) {
    Result v = stream.pulse(i * 1000);
    CHECK(v.count == i + 1);
    if (i)
      CHECK(near(v.ratePerSecond, 250));
  }
  stream.reset();
  CHECK(stream.poll(0).count == 0);
  CHECK(p.poll(20000).status == Status::InvalidTime);

  std::printf("PASS SuperJupiterPulseMetrics: %lu checks\n", checks);
}
