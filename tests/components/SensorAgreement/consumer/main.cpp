#include <SuperJupiter/SensorToolkit/SensorAgreement.h>
using namespace superjupiter::sensor_agreement;
int main() {
  Sample sensors[] = {{20, 0}, {20.2, 100}, {19.9, 50}};
  Result agreement = compare(sensors, 3, Config(0.5, 0, 1000, 2));
  (void)agreement;
  return 0;
}
