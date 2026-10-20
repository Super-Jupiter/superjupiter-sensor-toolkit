// SPDX-License-Identifier: MIT
// Synthetic input only; not a physical sensor driver. Hardware validation pending.
#include <Arduino.h>
#include <SuperJupiter/SensorToolkit/SensorQuality.h>
#include <SuperJupiter/SensorToolkit/SampleWindow.h>
superjupiter::sensor_quality::Config settings(){
 superjupiter::sensor_quality::Config c;
 c.rangeEnabled=true;c.minimum=-40;c.maximum=125;c.expectedIntervalMs=100;
 c.gapToleranceMs=20;c.staleAfterMs=500;return c;
}
superjupiter::sensor_quality::SensorQuality quality(settings());
superjupiter::sample_window::SampleWindow<4> history;
uint32_t last=0;
void setup(){Serial.begin(115200);}
void loop(){
 const uint32_t now=millis();
 if(now-last>=100){
  last=now;const float raw=20.0f; // Replace with a fresh driver result.
  const auto q=quality.observe(raw,now);
  const auto saved=history.push(raw,now);
  if(!saved.accepted){Serial.println("History rejected timestamp");return;}
  Serial.print("raw=");Serial.print(q.value);
  Serial.print(" flags=");Serial.println(q.flags);
 }
 // Continue checking freshness even when the sensor produces no new samples.
 if(quality.inspect(now).has(superjupiter::sensor_quality::Stale)){
  // Application decides whether to alarm, display missing data or stop a calculation.
 }
}
