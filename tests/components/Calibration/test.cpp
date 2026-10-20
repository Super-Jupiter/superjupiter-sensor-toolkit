#include <SuperJupiter/SensorToolkit/Calibration.h>
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
using namespace superjupiter::calibration;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  Point p[] = {{0, 10}, {100, 210}, {200, 310}};
  Calibration c(p, 3, 7);
  CHECK(c.valid());
  CHECK(c.evaluate(50).value == 110);
  CHECK(c.evaluate(100).value == 210);
  CHECK(c.evaluate(-1).status == Status::OutOfRange);
  CHECK(c.evaluate(NAN).status == Status::InvalidInput);
  CHECK(c.evaluate(20).raw == 20 && c.evaluate(20).revision == 7);
  Calibration clamp(p, 3, 0, Bounds::Clamp);
  CHECK(clamp.evaluate(300).value == 310);
  CHECK(clamp.evaluate(-1).status == Status::Clamped);
  Calibration ext(p, 3, 0, Bounds::Extrapolate);
  CHECK(ext.evaluate(300).value == 410);
  Point bad[] = {{0, 0}, {0, 1}};
  CHECK(!Calibration(bad, 2).valid());
  CHECK(!Calibration(0, 2).valid());
  Point huge[] = {{-1e308, 0}, {1e308, 1}};
  CHECK(Calibration(huge, 2).evaluate(0).status == Status::Ok);
  CHECK(Calibration(huge, 2).evaluate(0).value == 0.5);
  for (int i = 0; i <= 100; ++i)
    CHECK(near(c.evaluate(i).value, 10 + 2.0 * i));

  for (int trial = 1; trial <= 1000; ++trial) {
    const double x = trial / 10.0;
    CHECK(near(c.evaluate(x).value, 10 + 2 * x));
  }
  Point unordered[] = {{1, 0}, {0, 1}};
  CHECK(!Calibration(unordered, 2).valid());
  Point invalid[] = {{0, 0}, {1, NAN}};
  CHECK(!Calibration(invalid, 2).valid());
  CHECK(Calibration(p, 1).evaluate(1).status == Status::InvalidTable);

  std::printf("PASS SuperJupiterCalibration: %lu checks\n", checks);
}
