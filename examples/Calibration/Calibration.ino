#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/Calibration.h>
using namespace superjupiter::calibration;
void setup() {
  Serial.begin(115200);
  Point points[] = {{0, 0}, {4095, 100}};
  Calibration calibration(points, 2, 1);
  Result result = calibration.evaluate(2047.5);
  (void)result;
  Serial.println("SuperJupiter Calibration example ran. Hardware validation pending.");
}
void loop() {}
