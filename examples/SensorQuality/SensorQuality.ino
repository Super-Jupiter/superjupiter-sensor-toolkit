// SPDX-License-Identifier: MIT
// Synthetic values. Replace the value source only after configuring limits
// appropriate for your sensor. This is not a thermocouple driver.
#include <SuperJupiter/SensorToolkit/SensorQuality.h>

superjupiter::sensor_quality::Config settings() {
  superjupiter::sensor_quality::Config c;
  c.rangeEnabled = true;
  c.minimum = -40;
  c.maximum = 125;
  c.staleAfterMs = 1000;
  return c;
}

superjupiter::sensor_quality::SensorQuality monitor(settings());
uint32_t lastSampleAt = 0;

void setup() { Serial.begin(115200); }

void loop() {
  const uint32_t now = millis();
  if (now - lastSampleAt >= 100) {
    lastSampleAt = now;
    const float syntheticTemperature = 20.0f;
    const superjupiter::sensor_quality::Result r = monitor.observe(syntheticTemperature, now);
    Serial.print(r.value);
    Serial.print(" quality_flags=");
    Serial.println(r.flags);
  }
  // In a real application call this even when the driver has no fresh data.
  const superjupiter::sensor_quality::Result current = monitor.inspect(now);
  (void)current;
}
