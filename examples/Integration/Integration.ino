#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/Integration.h>
using namespace superjupiter::integration;
void setup() {
  Serial.begin(115200);
  Integration energy(5000);
  energy.push(100, 0);
  Result joules = energy.push(100, 1000);
  (void)joules;
  Serial.println("SuperJupiter Integration example ran. Hardware validation pending.");
}
void loop() {}
