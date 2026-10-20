#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/TimeAnchor.h>
using namespace superjupiter::time_anchor;
void setup() {
  Serial.begin(115200);
  TimeAnchor clock(Config(3600000, 100));
  clock.setAnchor(1000, 1700000000000LL, 20);
  Result result = clock.convert(1500);
  (void)result;
  Serial.println("SuperJupiter TimeAnchor example ran. Hardware validation pending.");
}
void loop() {}
