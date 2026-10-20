#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/SignalFilter.h>
using namespace superjupiter::signal_filter;
void setup() {
  Serial.begin(115200);
  SignalFilter filter(Config(1000, 5000));
  filter.push(20, 0);
  Result result = filter.push(21, 100);
  (void)result;
  Serial.println("SuperJupiter SignalFilter example ran. Hardware validation pending.");
}
void loop() {}
