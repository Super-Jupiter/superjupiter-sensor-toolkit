#include <SuperJupiter/SensorToolkit/MachineState.h>
using namespace superjupiter::machine_state;
int main() {
  MachineState machine(Config(2, 50, 1000, 5000));
  machine.push(100, 0);
  Result state = machine.push(100, 1000);
  (void)state;
  return 0;
}
