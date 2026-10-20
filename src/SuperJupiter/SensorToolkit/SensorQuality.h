// Consolidated public release baseline: SuperJupiterSensorQuality 0.2.1.
#if defined(SUPERJUPITER_SENSOR_QUALITY_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_SENSORQUALITY_H
#define SUPERJUPITER_SENSORTOOLKIT_SENSORQUALITY_H

#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error                                                                         \
    "SuperJupiter SensorQuality requires IEEE non-finite checks; disable fast-math/finite-math-only."
#endif

#define SUPERJUPITER_SENSOR_QUALITY_VERSION "0.2.1"

#include <float.h>
#include <stdint.h>

namespace superjupiter {
namespace sensor_quality {

enum Flag : uint16_t {
  None = 0,
  NoData = 1U << 0,
  InvalidNumber = 1U << 1,
  OutOfRange = 1U << 2,
  RateExceeded = 1U << 3,
  Stuck = 1U << 4,
  SampleGap = 1U << 5,
  Stale = 1U << 6,
  InvalidTimestamp = 1U << 7,
  InvalidConfiguration = 1U << 8,
  NumericError = 1U << 9
};

struct Config {
  bool rangeEnabled = false;
  float minimum = 0.0f;
  float maximum = 100.0f;
  float maxRatePerSecond = 0.0f;   // 0 disables rate checking.
  uint32_t expectedIntervalMs = 0; // 0 disables gap checking.
  uint32_t gapToleranceMs = 0;
  uint32_t staleAfterMs = 0;   // 0 disables staleness checking.
  uint32_t stuckAfterMs = 0;   // 0 disables stuck checking.
  float stuckTolerance = 0.0f; // Absolute distance from the run's first value.
};

struct Result {
  uint16_t flags = NoData;
  float value = 0.0f; // Unmodified last submitted value, including NaN/Inf.
  uint32_t sampledAtMs = 0;
  uint32_t intervalMs = 0; // Between observations, not sensor conversion time.
  double ratePerSecond = 0.0;
  bool hasReading = false;
  bool hasRate = false;

  bool has(Flag flag) const {
    return (flags & static_cast<uint16_t>(flag)) != 0;
  }
  bool healthy() const { return flags == None; }
};

// One instance per scalar sensor channel. No heap, I/O, clock reads or
// filtering. Call from one task; synchronise externally if sharing across tasks
// or ISRs.
class SensorQuality {
public:
  explicit SensorQuality(const Config &config = Config()) : config_(config) {
    reset();
  }

  static bool validConfig(const Config &config) {
    return (!config.rangeEnabled ||
            (finite(config.minimum) && finite(config.maximum) &&
             config.minimum <= config.maximum)) &&
           finite(config.maxRatePerSecond) && config.maxRatePerSecond >= 0.0f &&
           finite(config.stuckTolerance) && config.stuckTolerance >= 0.0f &&
           config.expectedIntervalMs <= maxInterval() &&
           config.gapToleranceMs <= maxInterval() - config.expectedIntervalMs &&
           config.staleAfterMs <= maxInterval() &&
           config.stuckAfterMs <= maxInterval() &&
           (config.expectedIntervalMs != 0 || config.gapToleranceMs == 0);
  }

  bool configured() const { return configured_; }
  const Config &config() const { return config_; }

  void reset() {
    configured_ = validConfig(config_);
    last_ = Result();
    if (!configured_)
      last_.flags |= InvalidConfiguration;
    haveObservation_ = false;
    haveUsable_ = false;
    havePrevious_ = false;
    haveAnchor_ = false;
    previousValue_ = 0.0f;
    anchorValue_ = 0.0f;
    previousAt_ = usableAgeMs_ = stuckAgeMs_ = 0;
  }

  // Timestamps must advance. A duplicate/backward timestamp is returned as an
  // error without changing state. A normal uint32_t millis() rollover is valid.
  Result observe(float value, uint32_t sampledAtMs) {
    Result result;
    result.flags = None;
    result.value = value;
    result.sampledAtMs = sampledAtMs;
    result.hasReading = true;
    if (!configured_) {
      result.flags = InvalidConfiguration;
      return result;
    }

    if (haveObservation_) {
      result.intervalMs = sampledAtMs - last_.sampledAtMs;
      if (result.intervalMs == 0 || result.intervalMs > maxInterval()) {
        result.flags = InvalidTimestamp;
        return result;
      }
    }
    if (haveUsable_)
      usableAgeMs_ = boundedAdd(usableAgeMs_, result.intervalMs);

    const bool gap =
        haveObservation_ && config_.expectedIntervalMs != 0 &&
        result.intervalMs > config_.expectedIntervalMs + config_.gapToleranceMs;
    if (gap)
      result.flags |= SampleGap;

    // A gap or invalid reading breaks continuity. We cannot infer a constant
    // signal, or its local rate, across observations we do not have.
    if (gap || (haveObservation_ && config_.staleAfterMs != 0 &&
                result.intervalMs >= config_.staleAfterMs)) {
      havePrevious_ = false;
      haveAnchor_ = false;
    }

    if (!finite(value)) {
      result.flags |= InvalidNumber;
    } else if (config_.rangeEnabled &&
               (value < config_.minimum || value > config_.maximum)) {
      result.flags |= OutOfRange;
    } else {
      haveUsable_ = true;
      usableAgeMs_ = 0;

      if (havePrevious_) {
        const uint32_t elapsed = sampledAtMs - previousAt_;
        const double rate = (static_cast<double>(value) - previousValue_) /
                            (static_cast<double>(elapsed) / 1000.0);
        result.hasRate = finiteDouble(rate);
        result.ratePerSecond = result.hasRate ? rate : 0.0;
        if (!result.hasRate)
          result.flags |= NumericError;
        if (config_.maxRatePerSecond != 0 &&
            (!result.hasRate || absolute(rate) > config_.maxRatePerSecond)) {
          result.flags |= RateExceeded;
        }
      }
      // A flagged jump becomes the next rate baseline, so a persistent step
      // does not permanently lock the channel into RateExceeded.
      previousValue_ = value;
      previousAt_ = sampledAtMs;
      havePrevious_ = true;

      if (!haveAnchor_ || absolute(static_cast<double>(value) - anchorValue_) >
                              config_.stuckTolerance) {
        anchorValue_ = value;
        stuckAgeMs_ = 0;
        haveAnchor_ = true;
      } else {
        stuckAgeMs_ = boundedAdd(stuckAgeMs_, result.intervalMs);
        if (config_.stuckAfterMs != 0 && stuckAgeMs_ >= config_.stuckAfterMs) {
          result.flags |= Stuck;
        }
      }
    }

    if (result.has(InvalidNumber) || result.has(OutOfRange)) {
      havePrevious_ = false;
      haveAnchor_ = false;
    }
    haveObservation_ = true;
    last_ = result;
    return inspect(sampledAtMs);
  }

  // Call even when the driver produces no new reading. Does not create a
  // sample or advance the clock. The value remains the last observed value.
  Result inspect(uint32_t nowMs) const {
    Result result = last_;
    if (!configured_)
      return result;
    if (!haveObservation_)
      return result;
    if (nowMs - last_.sampledAtMs > maxInterval()) {
      result.flags |= InvalidTimestamp;
      return result;
    }
    if (config_.staleAfterMs != 0 &&
        (!haveUsable_ || boundedAdd(usableAgeMs_, nowMs - last_.sampledAtMs) >=
                             config_.staleAfterMs)) {
      result.flags |= Stale;
    }
    return result;
  }

private:
  static uint32_t maxInterval() { return 0x7fffffffUL; }
  static uint32_t boundedAdd(uint32_t a, uint32_t b) {
    return b >= maxInterval() - a ? maxInterval() : a + b;
  }
  static bool finite(float value) {
    return value == value && value >= -FLT_MAX && value <= FLT_MAX;
  }
  static bool finiteDouble(double value) {
    return value == value && value >= -DBL_MAX && value <= DBL_MAX;
  }
  static double absolute(double value) { return value < 0 ? -value : value; }

  Config config_;
  Result last_;
  bool configured_ = false;
  bool haveObservation_ = false;
  bool haveUsable_ = false;
  bool havePrevious_ = false;
  bool haveAnchor_ = false;
  float previousValue_ = 0.0f;
  float anchorValue_ = 0.0f;
  uint32_t previousAt_ = 0;
  uint32_t usableAgeMs_ = 0;
  uint32_t stuckAgeMs_ = 0;
};

inline const char *flagName(Flag flag) {
  switch (flag) {
  case None:
    return "healthy";
  case NoData:
    return "no_data";
  case InvalidNumber:
    return "invalid_number";
  case OutOfRange:
    return "out_of_range";
  case RateExceeded:
    return "rate_exceeded";
  case Stuck:
    return "possibly_stuck";
  case SampleGap:
    return "sample_gap";
  case Stale:
    return "stale";
  case InvalidTimestamp:
    return "invalid_timestamp";
  case InvalidConfiguration:
    return "invalid_configuration";
  case NumericError:
    return "numeric_error";
  default:
    return "unknown";
  }
}

} // namespace sensor_quality
} // namespace superjupiter
#endif
