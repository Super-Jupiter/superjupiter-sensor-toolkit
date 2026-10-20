// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/MachineState.h>
#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
static unsigned checks = 0;
#define CHECK(x)                                                               \
  do {                                                                         \
    ++checks;                                                                  \
    if (!(x)) {                                                                \
      std::fprintf(stderr, "FAIL %s:%d %s\n", __FILE__, __LINE__, #x);         \
      std::abort();                                                            \
    }                                                                          \
  } while (false)
static bool close(double a, double b) {
  return std::fabs(a - b) <= 1e-12 * (1 + std::fabs(b));
}
using namespace superjupiter::machine_state;

int main() {
  (void)close;

  MachineState machine(Config(1, 2, 0, 1000));
  CHECK(machine.push(3, 0).state == State::Running);
  CHECK(machine.push(NAN, 100).state == State::Unknown);
  CHECK(machine.push(0, 99).status == Status::InvalidTime);
  CHECK(machine.push(0, 101).state == State::Stopped);
  CHECK(machine.push(NAN, 100).status == Status::InvalidTime);
  CHECK(machine.push(0, 201).state == State::Stopped);
  CHECK(machine.durationMs(State::Stopped) == 100);

  std::printf("PASS SuperJupiterMachineState review: %u checks\n", checks);
}
