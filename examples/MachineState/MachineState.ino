#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/MachineState.h>
using namespace superjupiter::machine_state;
void setup() {
  Serial.begin(115200);
  MachineState machine(Config(2, 50, 1000, 5000));
  machine.push(100, 0);
  Result state = machine.push(100, 1000);
  (void)state;
  Serial.println("SuperJupiter MachineState example ran. Hardware validation pending.");
}
void loop() {}
