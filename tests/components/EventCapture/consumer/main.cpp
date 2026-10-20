#include <SuperJupiter/SensorToolkit/EventCapture.h>
using namespace superjupiter::event_capture;
int main() {
  EventCapture<8, 4> capture;
  capture.push(20, 0);
  capture.push(21, 100);
  capture.trigger();
  capture.push(22, 200);
  return 0;
}
