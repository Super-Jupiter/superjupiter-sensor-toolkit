#include <SuperJupiter/SensorToolkit/ThresholdEvents.h>
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
using namespace superjupiter::threshold_events;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  ThresholdEvents a(Config(8, 10, 100, 200, 1000));
  CHECK(a.push(7, 0).state == State::Unknown);
  CHECK(a.push(7, 200).state == State::Normal);
  CHECK(a.push(11, 300).state == State::Normal);
  CHECK(a.push(11, 400).changed);
  CHECK(a.state() == State::Alarm);
  CHECK(a.push(9, 500).state == State::Alarm);
  a.push(7, 600);
  a.push(9, 700);
  a.push(7, 800);
  CHECK(a.push(7, 999).state == State::Alarm);
  CHECK(a.push(7, 1000).state == State::Normal);
  CHECK(a.push(NAN, 1001).state == State::Unknown);
  CHECK(a.push(7, 2000).state == State::Unknown);
  CHECK(a.push(7, 5000).status == Status::GapReset);
  CHECK(a.push(7, 5000).status == Status::InvalidTime);

  ThresholdEvents immediate(Config(1, 2, 0, 0, 1000));
  CHECK(immediate.push(2, UINT32_MAX - 10).state == State::Alarm);
  CHECK(immediate.push(1, 0).state == State::Normal);
  CHECK(immediate.push(1.5, 1).state == State::Normal);
  CHECK(ThresholdEvents(Config(1, 1)).push(1, 0).status == Status::InvalidConfig);
  ThresholdEvents debounce(Config(1, 2, 100, 100, 1000));
  for (uint32_t i = 0; i < 1000; ++i)
    CHECK(debounce.push(i % 2 ? 3 : 0, i * 10).state == State::Unknown);

  std::printf("PASS SuperJupiterThresholdEvents: %lu checks\n", checks);
}
