#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/EventCapture.h>
using namespace superjupiter::event_capture;
void setup() {
  Serial.begin(115200);
  EventCapture<8, 4> capture;
  capture.push(20, 0);
  capture.push(21, 100);
  capture.trigger();
  capture.push(22, 200);
  Serial.println("SuperJupiter EventCapture example ran. Hardware validation pending.");
}
void loop() {}
