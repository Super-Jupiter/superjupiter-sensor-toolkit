namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/Trend.h>
using namespace superjupiter::trend;
int main() {
  Trend<8> trend(Config(3, 200, 1000));
  trend.push(20, 0);
  trend.push(21, 100);
  trend.push(22, 200);
  Result result = trend.summary();
  (void)result;
  return 0;
}
