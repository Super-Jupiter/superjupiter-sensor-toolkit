#include <SuperJupiter/SensorToolkit/TimeAnchor.h>
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
using namespace superjupiter::time_anchor;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  TimeAnchor a(Config(10000, 1000));
  CHECK(a.convert(0).status == Status::NoAnchor);
  CHECK(a.setAnchor(UINT32_MAX - 9, 1000000, 2) == Status::Ok);
  Result r = a.convert(10);
  CHECK(r.status == Status::Ok && r.utcMs == 1000020 && r.uncertaintyMs == 3 && r.revision == 1);
  CHECK(a.convert(UINT32_MAX - 10).status == Status::OutsideHorizon);
  CHECK(a.convert(10001).status == Status::Expired);
  CHECK(a.setAnchor(0, -1, 0) == Status::InvalidAnchor);
  CHECK(a.convert(10).revision == 1);
  CHECK(a.setAnchor(0, INT64_MAX, 0) == Status::Ok);
  CHECK(a.convert(1).status == Status::Overflow);
  a.clear();
  CHECK(a.convert(0).status == Status::NoAnchor);
  TimeAnchor bad(Config(0x80000000UL));
  CHECK(bad.setAnchor(0, 0, 0) == Status::InvalidConfig);

  TimeAnchor precise(Config(1000000, 123));
  CHECK(precise.setAnchor(42, 1000000, 7) == Status::Ok);
  for (uint32_t dt = 0; dt < 1000000; dt += 997) {
    Result v = precise.convert(42 + dt);
    CHECK(v.utcMs == 1000000 + dt);
    CHECK(v.uncertaintyMs == 7 + (static_cast<uint64_t>(dt) * 123 + 999999) / 1000000);
  }
  CHECK(precise.convert(1000043).status == Status::Expired);

  std::printf("PASS SuperJupiterTimeAnchor: %lu checks\n", checks);
}
