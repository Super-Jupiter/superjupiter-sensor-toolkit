#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/ChannelAlign.h>
using namespace superjupiter::channel_align;
void setup() {
  Serial.begin(115200);
  ChannelAlign<2> channels(1000, 100);
  channels.push(0, 20, 0);
  channels.push(0, 22, 1000);
  Result aligned = channels.at(0, 500);
  (void)aligned;
  Serial.println("SuperJupiter ChannelAlign example ran. Hardware validation pending.");
}
void loop() {}
