#include <SuperJupiter/SensorToolkit/PulseMetrics.h>
using namespace superjupiter::pulse_metrics;
int main() {
  PulseMetrics meter(Config(100, 1000000, 0.5));
  meter.pulse(0);
  Result result = meter.pulse(100000);
  (void)result;
  return 0;
}
