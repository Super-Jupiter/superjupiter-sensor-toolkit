namespace superjupiter {
struct Config {};
struct Result {};
namespace unrelated {
struct State {};
} // namespace unrelated
} // namespace superjupiter
#include <SuperJupiter/SensorToolkit/TimeAggregate.h>
using namespace superjupiter::time_aggregate;
int main() {
  TimeAggregate minutes(60000);
  minutes.push(20, 0);
  minutes.push(21, 1000);
  minutes.push(22, 60000);
  Summary previous;
  minutes.takeCompleted(previous);
  return 0;
}
