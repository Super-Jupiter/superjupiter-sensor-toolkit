#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/Trend.h>
using namespace superjupiter::trend;
void setup() {
  Serial.begin(115200);
  Trend<8> trend(Config(3, 200, 1000));
  trend.push(20, 0);
  trend.push(21, 100);
  trend.push(22, 200);
  Result result = trend.summary();
  (void)result;
  Serial.println("SuperJupiter Trend example ran. Hardware validation pending.");
}
void loop() {}
