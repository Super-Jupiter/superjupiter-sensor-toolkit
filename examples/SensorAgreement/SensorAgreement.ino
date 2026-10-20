#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/SensorAgreement.h>
using namespace superjupiter::sensor_agreement;
void setup() {
  Serial.begin(115200);
  Sample sensors[] = {{20, 0}, {20.2, 100}, {19.9, 50}};
  Result agreement = compare(sensors, 3, Config(0.5, 0, 1000, 2));
  (void)agreement;
  Serial.println("SuperJupiter SensorAgreement example ran. Hardware validation pending.");
}
void loop() {}
