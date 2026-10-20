// SPDX-License-Identifier: MIT
#include "SuperJupiter/SensorToolkit/SensorQuality.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

using namespace superjupiter::sensor_quality;
static unsigned checks = 0;
#define CHECK(expression) do { ++checks; if (!(expression)) { \
  std::cerr << __FILE__ << ':' << __LINE__ << ": " #expression "\n"; std::exit(1); \
} } while (false)

static Config configured() {
  Config c;
  c.rangeEnabled = true;
  c.minimum = -40.0f;
  c.maximum = 125.0f;
  c.expectedIntervalMs = 100;
  c.gapToleranceMs = 20;
  c.staleAfterMs = 500;
  c.maxRatePerSecond = 10.0f;
  c.stuckAfterMs = 1000;
  c.stuckTolerance = 0.1f;
  return c;
}

static void initial_and_reset() {
  SensorQuality q(configured());
  CHECK(q.configured());
  CHECK(q.inspect(0).has(NoData));
  CHECK(!q.inspect(500).hasReading);
  CHECK(!q.inspect(500).has(Stale));
  Result a = q.observe(20.0f, 0);
  CHECK(a.healthy());
  CHECK(a.hasReading);
  CHECK(!a.hasRate);
  CHECK(a.value == 20.0f);
  CHECK(a.sampledAtMs == 0);
  CHECK(q.inspect(499).healthy());
  CHECK(q.inspect(500).has(Stale));
  q.reset();
  CHECK(q.inspect(2000).flags == NoData);
  CHECK(q.observe(20.0f, 0).healthy());
}

static void range_and_invalid_values() {
  Config c = configured();
  c.maxRatePerSecond = 0;
  SensorQuality q(c);
  CHECK(q.observe(-40.0f, 0).healthy());
  CHECK(q.observe(125.0f, 100).healthy());
  Result out = q.observe(125.5f, 200);
  CHECK(out.has(OutOfRange));
  CHECK(out.value == 125.5f);
  CHECK(!out.has(Stale));
  CHECK(q.inspect(600).has(Stale));
  const float bad[] = {std::numeric_limits<float>::quiet_NaN(),
    std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity()};
  for (unsigned i = 0; i < 3; ++i) {
    q.reset();
    Result r = q.observe(bad[i], 100);
    CHECK(r.has(InvalidNumber));
    CHECK(r.has(Stale));
    CHECK(r.hasReading);
    CHECK(!r.hasRate);
    CHECK((std::isnan(bad[i]) && std::isnan(r.value)) || r.value == bad[i]);
    CHECK(!q.observe(20.0f, 200).has(Stale));
  }
  SensorQuality defaults;
  CHECK(defaults.observe(-std::numeric_limits<float>::max(), 0).healthy());
  CHECK(defaults.observe(std::numeric_limits<float>::max(), 1).healthy());
  CHECK(defaults.observe(bad[0], 2).has(InvalidNumber));
  CHECK(!defaults.inspect(100000).has(Stale));
}

static void rates_and_recovery() {
  Config c = configured();
  c.stuckAfterMs = 0;
  SensorQuality q(c);
  q.observe(20.0f, 0);
  Result exact = q.observe(21.0f, 100);
  CHECK(exact.hasRate);
  CHECK(std::fabs(exact.ratePerSecond - 10.0) < 0.00001);
  CHECK(!exact.has(RateExceeded));
  CHECK(q.observe(23.0f, 200).has(RateExceeded));
  Result recovered = q.observe(23.0f, 300);
  CHECK(!recovered.has(RateExceeded));
  CHECK(recovered.hasRate);
  CHECK(recovered.ratePerSecond == 0);
  Result negative = q.observe(20.0f, 400);
  CHECK(negative.has(RateExceeded));
  CHECK(negative.ratePerSecond < 0);
  q.observe(std::numeric_limits<float>::quiet_NaN(), 500);
  CHECK(!q.observe(30.0f, 600).hasRate);
  q.observe(200.0f, 700);
  CHECK(!q.observe(0.0f, 800).hasRate);
  Result gap = q.observe(100.0f, 1500);
  CHECK(gap.has(SampleGap));
  CHECK(!gap.hasRate);
  CHECK(!gap.has(RateExceeded));
}

static void gaps_and_staleness() {
  SensorQuality q(configured());
  q.observe(20.0f, 100);
  CHECK(!q.observe(20.0f, 220).has(SampleGap));
  Result gap = q.observe(20.0f, 341);
  CHECK(gap.has(SampleGap));
  CHECK(gap.intervalMs == 121);
  CHECK(q.inspect(500).has(SampleGap));
  CHECK(!q.inspect(500).has(Stale));
  CHECK(q.observe(20.0f, 441).healthy());
  CHECK(q.inspect(941).has(Stale));
  CHECK(!q.inspect(940).has(Stale));
  CHECK(q.inspect(941).sampledAtMs == 441);
  q.observe(200.0f, 541);
  CHECK(q.inspect(941).has(OutOfRange));
  CHECK(q.inspect(941).has(Stale));
  Result recovery = q.observe(20.0f, 1000);
  CHECK(!recovery.has(Stale));
  CHECK(recovery.has(SampleGap));
  CHECK(q.observe(20.0f, 1100).healthy());
}

static void stuck_is_a_suspicion() {
  Config c = configured();
  c.stuckAfterMs = 300;
  c.stuckTolerance = 0.1f;
  SensorQuality q(c);
  q.observe(20.0f, 0);
  q.observe(20.02f, 100);
  CHECK(!q.observe(20.04f, 200).has(Stuck));
  CHECK(q.observe(20.05f, 300).has(Stuck));
  CHECK(q.inspect(350).has(Stuck));
  CHECK(!q.observe(20.2f, 400).has(Stuck));
  CHECK(!q.observe(20.2f, 1000).has(Stuck));  // Gap breaks continuity.
  q.observe(20.2f, 1100);
  q.observe(std::numeric_limits<float>::quiet_NaN(), 1200);
  CHECK(!q.observe(20.2f, 1300).has(Stuck));

  // Compare with the run's anchor, not just the previous value. A slowly
  // changing signal must not look stuck merely because each step is small.
  q.reset();
  for (uint32_t i = 0; i < 20; ++i) {
    CHECK(!q.observe(20.0f + 0.06f * static_cast<float>(i), i * 100).has(Stuck));
  }
  c.stuckTolerance = 0;
  SensorQuality exact(c);
  exact.observe(20.0f, 0);
  exact.observe(20.0f, 100);
  exact.observe(20.0f, 200);
  CHECK(exact.observe(20.0f, 300).has(Stuck));
  CHECK(!exact.observe(20.0001f, 400).has(Stuck));
}

static void timestamp_contract() {
  SensorQuality q(configured());
  q.observe(20.0f, 100);
  CHECK(q.observe(21.0f, 100).has(InvalidTimestamp));
  CHECK(q.observe(21.0f, 99).has(InvalidTimestamp));
  CHECK(q.inspect(99).has(InvalidTimestamp));
  CHECK(q.inspect(100).value == 20.0f);
  CHECK(q.inspect(100).sampledAtMs == 100);
  CHECK(q.observe(21.0f, 200).intervalMs == 100);
  q.reset();
  q.observe(20.0f, 0xfffffff0UL);
  Result wrap = q.observe(20.0f, 0x00000054UL);
  CHECK(wrap.intervalMs == 100);
  CHECK(wrap.healthy());
  CHECK(!q.inspect(0x00000055UL).has(InvalidTimestamp));
  CHECK(q.inspect(0x00000248UL).has(Stale));
  q.reset();
  q.observe(20.0f, 0);
  CHECK(q.observe(20.0f, 0x80000000UL).has(InvalidTimestamp));
  CHECK(q.inspect(0).sampledAtMs == 0);

  // Multiple clock rollovers must not clear an established stale/stuck flag.
  Config c;
  c.stuckAfterMs = 100;
  SensorQuality longRun(c);
  c.staleAfterMs = 500;
  SensorQuality invalidRun(c);
  longRun.observe(20, 0);
  invalidRun.observe(20, 0);
  uint32_t time = 0;
  for (unsigned i = 0; i < 10; ++i) {
    time += 1000000000UL;  // Each interval is below the half-range limit.
    CHECK(longRun.observe(20, time).has(Stuck));
    CHECK(invalidRun.observe(std::numeric_limits<float>::quiet_NaN(), time).has(Stale));
  }
}

static void bad_configuration() {
  Config c = configured();
  c.minimum = 126.0f;
  CHECK(!SensorQuality::validConfig(c));
  SensorQuality q(c);
  CHECK(!q.configured());
  CHECK(q.inspect(0).has(InvalidConfiguration));
  CHECK(q.observe(20.0f, 100).has(InvalidConfiguration));
  CHECK(!q.inspect(100).hasReading);
  c = configured(); c.minimum = std::numeric_limits<float>::quiet_NaN();
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.maxRatePerSecond = -1;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.maxRatePerSecond = std::numeric_limits<float>::infinity();
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.stuckTolerance = -0.1f;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.expectedIntervalMs = 0;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.expectedIntervalMs = 0x7fffffffUL; c.gapToleranceMs = 1;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.expectedIntervalMs = 0x80000000UL;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.staleAfterMs = 0x80000000UL;
  CHECK(!SensorQuality::validConfig(c));
  c = configured(); c.stuckAfterMs = 0x80000000UL;
  CHECK(!SensorQuality::validConfig(c));
  c = Config(); c.rangeEnabled = true; c.minimum = c.maximum = 0;
  CHECK(SensorQuality::validConfig(c));
  CHECK(SensorQuality(c).observe(0, 0).healthy());
}

static void long_stream_and_independent_channels() {
  Config c;
  c.rangeEnabled = true;
  c.minimum = -100;
  c.maximum = 100;
  c.expectedIntervalMs = 100;
  c.maxRatePerSecond = 20;
  c.staleAfterMs = 1000;
  SensorQuality q(c), other(c);
  uint32_t time = 0xffff0000UL;
  for (unsigned i = 0; i < 100000; ++i) {
    const float value = 20.0f + static_cast<float>(i % 10) * 0.01f;
    const Result result = q.observe(value, time);
    CHECK(result.healthy());
    CHECK(result.value == value);
    time += 100;
  }
  CHECK(other.inspect(time).has(NoData));
  CHECK(other.observe(0, 0).healthy());
  CHECK(q.inspect(time).value != 0);
  CHECK(flagName(Stuck) == std::string("possibly_stuck"));
}

static void half_range_and_saturation_boundaries() {
  const uint32_t limit = 0x7fffffffUL;
  Config c;
  c.expectedIntervalMs = limit;
  c.staleAfterMs = limit;
  c.stuckAfterMs = limit;
  CHECK(SensorQuality::validConfig(c));
  SensorQuality q(c);
  q.observe(1.0f, 0);
  CHECK(!q.inspect(limit - 1).has(Stale));
  CHECK(q.inspect(limit).has(Stale));
  Result boundary = q.observe(1.0f, limit);
  CHECK(!boundary.has(InvalidTimestamp));
  CHECK(!boundary.has(SampleGap));
  CHECK(!boundary.hasRate);  // Stale-length interruption breaks continuity.
  q.reset();
  q.observe(1.0f, 0);
  CHECK(q.observe(2.0f, limit + 1).has(InvalidTimestamp));
  CHECK(q.inspect(0).value == 1.0f);

  Config ages;
  ages.staleAfterMs = 100;
  ages.stuckAfterMs = 100;
  SensorQuality stale(ages);
  stale.observe(1.0f, 0);
  uint32_t at = 0;
  for (unsigned i = 0; i < 8; ++i) {
    at += 1000000000UL;
    CHECK(stale.observe(std::numeric_limits<float>::quiet_NaN(), at).has(Stale));
  }
  ages.staleAfterMs = 0;
  SensorQuality longStuck(ages);
  longStuck.observe(1.0f, 0);
  at = 0;
  for (unsigned i = 0; i < 8; ++i) {
    at += 1000000000UL;
    CHECK(longStuck.observe(1.0f, at).has(Stuck));
  }
  CHECK(!longStuck.observe(2.0f, at + 1).has(Stuck));
}

static void numeric_and_combined_flag_boundaries() {
  Config c;
  c.maxRatePerSecond = 1;
  c.staleAfterMs = 500;
  SensorQuality q(c);
  q.observe(-std::numeric_limits<float>::max(), 0);
  Result extreme = q.observe(std::numeric_limits<float>::max(), 1);
  CHECK(extreme.has(RateExceeded));
  CHECK(extreme.value == std::numeric_limits<float>::max());
  // A target with float-width double may overflow; the flag is still required.
  CHECK(!extreme.hasRate || extreme.ratePerSecond > 1.0);
  q.reset();
  CHECK(q.observe(-0.0f, 0).healthy());
  CHECK(std::signbit(q.inspect(0).value));
  CHECK(q.observe(0.0f, 1).ratePerSecond == 0.0);
  Config all = configured();
  SensorQuality combined(all);
  combined.observe(20.0f, 0);
  Result bad = combined.observe(200.0f, 500);
  CHECK(bad.has(OutOfRange));
  CHECK(bad.has(SampleGap));
  CHECK(bad.has(Stale));
  CHECK(!bad.hasRate);
  Result rejected = combined.observe(21.0f, 500);
  CHECK(rejected.has(InvalidTimestamp));
  CHECK(combined.inspect(500).value == 200.0f);
  CHECK(combined.inspect(500).has(OutOfRange));
  CHECK(!combined.observe(21.0f, 501).hasRate);
  CHECK(std::string(flagName(None)) == "healthy");
  CHECK(std::string(flagName(static_cast<Flag>(0xffff))) == "unknown");
}

int main() {
  initial_and_reset();
  range_and_invalid_values();
  rates_and_recovery();
  gaps_and_staleness();
  stuck_is_a_suspicion();
  timestamp_contract();
  bad_configuration();
  long_stream_and_independent_channels();
  half_range_and_saturation_boundaries();
  numeric_and_combined_flag_boundaries();
  std::cout << "PASS: " << checks << " checks; SensorQuality instance: "
            << sizeof(SensorQuality) << " bytes on this host\n";
}
