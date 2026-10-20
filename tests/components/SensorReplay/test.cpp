#include <SuperJupiter/SensorToolkit/testing/SensorReplay.h>
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
using namespace superjupiter::sensor_replay;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  Sample s[] = {{1, 0, 0}, {2, 100, 1}, {3, 200, 0}, {NAN, 200, 1}};
  Fault faults[] = {{1, FaultKind::Drop, 0}, {2, FaultKind::Replace, 99}};
  SensorReplay r(s, 4, faults, 2);
  Output out = {};
  CHECK(r.next(0, out) == Status::NotStarted);
  CHECK(r.start(UINT32_MAX - 49) == Status::Ready);
  CHECK(r.next(UINT32_MAX - 49, out) == Status::SampleReady && out.raw == 1);
  CHECK(r.next(0, out) == Status::Waiting);
  CHECK(r.next(150, out) == Status::SampleReady && out.raw == 99 && out.atMs == 150 &&
        out.sourceIndex == 2);
  CHECK(r.next(150, out) == Status::SampleReady && std::isnan(out.raw));
  CHECK(r.next(150, out) == Status::Finished);
  CHECK(r.next(149, out) == Status::InvalidTime);
  CHECK(r.start(0) == Status::Ready);
  CHECK(r.next(0, out) == Status::SampleReady && out.raw == 1);
  Fault bad[] = {{9, FaultKind::Drop, 0}};
  CHECK(!SensorReplay(s, 4, bad, 1).valid());
  Sample unordered[] = {{1, 2, 0}, {2, 1, 0}};
  CHECK(!SensorReplay(unordered, 2).valid());

  Sample many[100];
  for (size_t i = 0; i < 100; ++i)
    many[i] = Sample{static_cast<double>(i), static_cast<uint32_t>(i * 10), 0};
  SensorReplay stream(many, 100);
  stream.start(0);
  for (size_t i = 0; i < 100; ++i) {
    CHECK(stream.next(1000, out) == Status::SampleReady);
    CHECK(out.sourceIndex == i && out.raw == static_cast<double>(i) && out.atMs == i * 10);
  }
  CHECK(stream.next(1000, out) == Status::Finished);
  CHECK(stream.consumed() == 100);

  std::printf("PASS SuperJupiterSensorReplay: %lu checks\n", checks);
}
