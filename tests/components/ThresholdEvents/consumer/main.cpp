#include <SuperJupiter/SensorToolkit/ThresholdEvents.h>
using namespace superjupiter::threshold_events;
int main() {
  ThresholdEvents alarm(Config(28, 30, 1000, 2000, 5000));
  alarm.push(31, 0);
  Result result = alarm.push(31, 1000);
  (void)result;
  return 0;
}
