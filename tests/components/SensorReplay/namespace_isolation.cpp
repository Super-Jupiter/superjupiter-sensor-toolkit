namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/testing/SensorReplay.h>
using namespace superjupiter::sensor_replay;
int main() {
  Sample samples[] = {{20, 0, 0}, {21, 100, 0}, {22, 200, 0}};
  SensorReplay replay(samples, 3);
  replay.start(0);
  Output reading = {};
  replay.next(100, reading);
  return 0;
}
