#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/testing/SensorReplay.h>
using namespace superjupiter::sensor_replay;
void setup() {
  Serial.begin(115200);
  Sample samples[] = {{20, 0, 0}, {21, 100, 0}, {22, 200, 0}};
  SensorReplay replay(samples, 3);
  replay.start(0);
  Output reading = {};
  replay.next(100, reading);
  Serial.println("SuperJupiter SensorReplay example ran. Hardware validation pending.");
}
void loop() {}
