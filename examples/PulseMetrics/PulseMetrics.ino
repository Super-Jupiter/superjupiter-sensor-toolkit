#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/PulseMetrics.h>
using namespace superjupiter::pulse_metrics;
void setup() {
  Serial.begin(115200);
  PulseMetrics meter(Config(100, 1000000, 0.5));
  meter.pulse(0);
  Result result = meter.pulse(100000);
  (void)result;
  Serial.println("SuperJupiter PulseMetrics example ran. Hardware validation pending.");
}
void loop() {}
