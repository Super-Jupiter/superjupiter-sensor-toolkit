#include <SuperJupiter/SensorToolkit/SensorAgreement.h>
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
using namespace superjupiter::sensor_agreement;
void *operator new(std::size_t) { std::abort(); }
void *operator new[](std::size_t) { std::abort(); }
void operator delete(void *p) noexcept { std::free(p); }
void operator delete[](void *p) noexcept { std::free(p); }
int main() {
  (void)near;
  Sample s[] = {{20, 0}, {20.5, 10}, {NAN, 0}, {99, 1001}};
  Result r = compare(s, 4);
  CHECK(r.status == Status::Agree && r.eligible == 2 && r.excluded == 2 && r.mean == 20.25);
  s[1].value = 22;
  CHECK(compare(s, 4).status == Status::Disagree);
  CHECK(compare(s, 1).status == Status::Insufficient);
  CHECK(compare(0, 1).status == Status::InvalidConfig);
  CHECK(compare(s, 2, Config(0, 0.1)).status == Status::Agree);
  Sample huge[] = {{-1e308, 0}, {1e308, 0}};
  CHECK(compare(huge, 2).status == Status::NumericError);

  Sample repeated[10];
  for (size_t i = 0; i < 10; ++i)
    repeated[i] = Sample{42, 0};
  for (size_t n = 2; n <= 10; ++n) {
    Result v = compare(repeated, n, Config(0, 0));
    CHECK(v.status == Status::Agree && near(v.mean, 42) && v.spread == 0);
  }
  CHECK(compare(repeated, 10, Config(-1)).status == Status::InvalidConfig);

  std::printf("PASS SuperJupiterSensorAgreement: %lu checks\n", checks);
}
