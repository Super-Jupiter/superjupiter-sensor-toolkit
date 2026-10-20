#include <SuperJupiter/SensorToolkit/ChannelAlign.h>
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
using namespace superjupiter::channel_align;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  ChannelAlign<2> a(1000, 100);
  CHECK(a.at(0, 0).status == Status::Missing);
  a.push(0, 0, 0);
  a.push(0, 10, 1000);
  CHECK(a.at(0, 500).value == 5);
  CHECK(a.at(0, 1000).status == Status::Ok);
  CHECK(a.at(0, 1100).status == Status::Held);
  CHECK(a.at(0, 1101).status == Status::Stale);
  CHECK(a.push(0, 0, 1000) == Status::InvalidTime);
  CHECK(a.push(2, 0, 0) == Status::InvalidChannel);
  CHECK(a.push(0, NAN, 2000) == Status::InvalidInput);
  a.push(1, 1, 0);
  a.push(1, 3, 2000);
  CHECK(a.at(1, 500).status == Status::TooWide);
  ChannelAlign<1> w(100);
  w.push(0, 0, UINT32_MAX - 49);
  w.push(0, 100, 50);
  CHECK(w.at(0, 0).value == 50);

  ChannelAlign<1> line(1000, 0);
  line.push(0, 10, 100);
  line.push(0, 30, 1100);
  for (uint32_t offset = 0; offset <= 1000; offset += 10)
    CHECK(near(line.at(0, 100 + offset).value, 10 + 0.02 * offset));
  CHECK(line.at(0, 99).status == Status::Missing);

  std::printf("PASS SuperJupiterChannelAlign: %lu checks\n", checks);
}
