#include <SuperJupiter/SensorToolkit/MachineState.h>
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
using namespace superjupiter::machine_state;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  MachineState m(Config(1, 10, 100, 1000));
  CHECK(m.push(0, 0).state == State::Unknown);
  CHECK(m.push(0, 100).state == State::Stopped);
  m.push(20, 200);
  CHECK(m.push(20, 300).state == State::Running);
  CHECK(m.durationMs(State::Stopped) == 200);
  m.push(5, 400);
  CHECK(m.push(5, 500).state == State::Idle);
  CHECK(m.durationMs(State::Running) == 200);
  CHECK(m.push(0, 500).status == Status::InvalidTime);
  CHECK(m.push(0, 2000).status == Status::Gap);
  CHECK(m.durationMs(State::Idle) == 0);
  CHECK(m.push(NAN, 2001).state == State::Unknown);

  MachineState steady(Config(1, 10, 0, 1000));
  for (uint32_t i = 0; i <= 1000; ++i)
    CHECK(steady.push(20, i * 100).state == State::Running);
  CHECK(steady.durationMs(State::Running) == 100000);
  CHECK(steady.durationMs(State::Unknown) == 0);
  CHECK(MachineState(Config(2, 1)).push(0, 0).status == Status::InvalidConfig);

  std::printf("PASS SuperJupiterMachineState: %lu checks\n", checks);
}
