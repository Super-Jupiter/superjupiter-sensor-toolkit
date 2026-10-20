// SPDX-License-Identifier: MIT
#include <SuperJupiter/SensorToolkit/SensorQuality.h>
int main() {
  superjupiter::sensor_quality::Config config;
  config.staleAfterMs = 100;
  superjupiter::sensor_quality::SensorQuality monitor(config);
  if (!monitor.observe(20.0f, 0).healthy()) return 1;
  return monitor.inspect(100).has(superjupiter::sensor_quality::Stale) ? 0 : 2;
}
