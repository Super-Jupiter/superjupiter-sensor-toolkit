#include <SuperJupiter/SensorToolkit/EventCapture.h>
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
using namespace superjupiter::event_capture;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  EventCapture<2, 2> c;
  c.push(1, 0);
  c.push(2, 1);
  c.push(NAN, 2);
  CHECK(c.trigger() == Status::Capturing);
  CHECK(c.trigger() == Status::Busy);
  CHECK(c.push(3, 2) == Status::InvalidTime);
  CHECK(c.push(3, 3) == Status::Capturing);
  CHECK(c.push(4, 4) == Status::Complete);
  Sample s;
  CHECK(c.size() == 4 && c.complete());
  CHECK(c.get(0, s) && s.raw == 2);
  CHECK(c.get(1, s) && std::isnan(s.raw));
  CHECK(c.get(3, s) && s.raw == 4);
  CHECK(!c.get(4, s));
  c.push(99, 5);
  CHECK(c.get(3, s) && s.raw == 4);
  c.reset();
  c.trigger();
  c.push(1, 0);
  CHECK(c.push(2, 1) == Status::Complete && c.size() == 2);

  EventCapture<4, 3> event;
  for (uint32_t i = 0; i < 1000; ++i)
    event.push(i, i);
  CHECK(event.trigger() == Status::Capturing);
  event.push(1000, 1000);
  event.push(1001, 1001);
  CHECK(event.push(1002, 1002) == Status::Complete);
  for (size_t i = 0; i < 7; ++i) {
    Sample sample;
    CHECK(event.get(i, sample));
    CHECK(sample.raw == static_cast<double>(996 + i));
  }

  std::printf("PASS SuperJupiterEventCapture: %lu checks\n", checks);
}
