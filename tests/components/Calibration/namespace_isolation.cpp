namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/Calibration.h>
using namespace superjupiter::calibration;
int main() {
  Point points[] = {{0, 0}, {4095, 100}};
  Calibration calibration(points, 2, 1);
  Result result = calibration.evaluate(2047.5);
  (void)result;
  return 0;
}
