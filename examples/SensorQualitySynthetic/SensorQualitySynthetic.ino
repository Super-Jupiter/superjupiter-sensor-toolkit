// SPDX-License-Identifier: MIT
// No sensor required. All temperatures and faults below are SYNTHETIC.
#include <SuperJupiter/SensorToolkit/SensorQuality.h>
#include <math.h>

superjupiter::sensor_quality::Config temperatureConfig() {
  superjupiter::sensor_quality::Config config;
  config.rangeEnabled = true;
  config.minimum = -40;
  config.maximum = 125;
  config.maxRatePerSecond = 10;
  config.expectedIntervalMs = 100;
  config.gapToleranceMs = 30;
  config.staleAfterMs = 750;
  config.stuckAfterMs = 2000;
  config.stuckTolerance = 0.02f;
  return config;
}

superjupiter::sensor_quality::SensorQuality quality(temperatureConfig());
uint32_t lastTick = 0;
uint32_t step = 0;

void printResult(const superjupiter::sensor_quality::Result& result) {
  Serial.print(result.sampledAtMs);
  Serial.print(',');
  Serial.print(result.value, 3);
  Serial.print(',');
  if (result.healthy()) Serial.print("healthy");
  const superjupiter::sensor_quality::Flag flags[] = {
    superjupiter::sensor_quality::NoData, superjupiter::sensor_quality::InvalidNumber, superjupiter::sensor_quality::OutOfRange,
    superjupiter::sensor_quality::RateExceeded, superjupiter::sensor_quality::Stuck, superjupiter::sensor_quality::SampleGap,
    superjupiter::sensor_quality::Stale, superjupiter::sensor_quality::InvalidTimestamp, superjupiter::sensor_quality::InvalidConfiguration
  };
  bool separator = false;
  for (unsigned i = 0; i < sizeof(flags) / sizeof(flags[0]); ++i) {
    if (result.has(flags[i])) {
      if (separator) Serial.print('|');
      Serial.print(superjupiter::sensor_quality::flagName(flags[i]));
      separator = true;
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("SuperJupiter SensorQuality: synthetic temperature and fault demo");
  Serial.println("sampled_at_ms,raw_value,flags");
}

void loop() {
  const uint32_t now = millis();
  if (now - lastTick < 100) return;
  lastTick = now;
  const uint32_t phase = step++ % 120;
  if (phase >= 90 && phase < 106) {
    // Simulate a driver that is producing NO new reading. inspect() will keep
    // the last sample timestamp and eventually report stale. Never resubmit
    // a cached reading with a new timestamp to make it appear fresh.
    printResult(quality.inspect(now));
    return;
  }
  float value = 20.0f + 0.1f * static_cast<float>(phase % 5);
  if (phase == 10) value = 80;  // Sudden jump, still within range.
  if (phase >= 20 && phase < 25) value = NAN;
  if (phase >= 30 && phase < 35) value = 200;  // Outside example limits.
  if (phase >= 50 && phase < 85) value = 22;  // Possibly stuck.
  printResult(quality.observe(value, now));
}
