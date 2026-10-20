// SPDX-License-Identifier: MIT
// Deliberate neighbouring names ensure the public header does not claim them.
namespace superjupiter {
struct Config {};
struct Result {};
enum Flag { None };
namespace other_component {
struct Config {};
struct Result {};
enum Flag { None };
}
}
#include <SuperJupiter/SensorToolkit/SensorQuality.h>

int main() {
  superjupiter::sensor_quality::Config config;
  superjupiter::sensor_quality::SensorQuality monitor(config);
  return monitor.observe(20.0f, 0).healthy() ? 0 : 1;
}
