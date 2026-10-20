#include <SuperJupiter/SensorToolkit/TimeAggregate.h>
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
using namespace superjupiter::time_aggregate;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  TimeAggregate a(1000);
  Summary s;
  CHECK(!a.takeCompleted(s));
  a.push(2, 0);
  a.push(4, 500);
  a.push(NAN, 600);
  CHECK(a.push(6, 1000) == Status::Completed);
  CHECK(a.takeCompleted(s));
  CHECK(s.mean == 3 && s.finiteCount == 2 && s.invalidCount == 1);
  CHECK(s.coverage == 0.5);
  CHECK(!a.takeCompleted(s));
  CHECK(a.push(9, 4000) == Status::Completed);
  CHECK(a.skippedBuckets() == 2);
  CHECK(a.takeCompleted(s) && s.startMs == 1000 && s.mean == 6);
  CHECK(a.push(0, 4000) == Status::InvalidTime);
  TimeAggregate wrap(100);
  wrap.push(1, UINT32_MAX - 49);
  CHECK(wrap.push(2, 50) == Status::Completed);
  CHECK(wrap.current().startMs == 50);
  CHECK(TimeAggregate(0).push(0, 0) == Status::InvalidConfig);

  TimeAggregate stream(100);
  Summary previous;
  unsigned completed = 0;
  for (uint32_t i = 0; i < 1000; ++i) {
    Status status = stream.push(static_cast<double>(i), i * 10);
    if (status == Status::Completed) {
      CHECK(stream.takeCompleted(previous));
      CHECK(previous.observations == 10);
      CHECK(near(previous.mean, completed * 10 + 4.5));
      ++completed;
    }
  }
  CHECK(completed == 99);
  CHECK(stream.current().observations == 10);

  std::printf("PASS SuperJupiterTimeAggregate: %lu checks\n", checks);
}
