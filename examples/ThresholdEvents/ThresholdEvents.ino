#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/ThresholdEvents.h>
using namespace superjupiter::threshold_events;
void setup() {
  Serial.begin(115200);
  ThresholdEvents alarm(Config(28, 30, 1000, 2000, 5000));
  alarm.push(31, 0);
  Result result = alarm.push(31, 1000);
  (void)result;
  Serial.println("SuperJupiter ThresholdEvents example ran. Hardware validation pending.");
}
void loop() {}
