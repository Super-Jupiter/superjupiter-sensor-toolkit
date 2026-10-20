// Consolidated public release baseline: SuperJupiterPulseMetrics 0.1.1.
#if defined(SUPERJUPITER_PULSE_METRICS_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_PULSEMETRICS_H
#define SUPERJUPITER_SENSORTOOLKIT_PULSEMETRICS_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_PULSE_METRICS_VERSION "0.1.1"
namespace superjupiter {
namespace pulse_metrics {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
} // namespace detail

enum class Status {
  First,
  Ok,
  Bounce,
  InvalidTime,
  InvalidConfig,
  NoRate,
  TimedOut,
  Overflow
};
struct Config {
  uint32_t minIntervalUs, timeoutUs;
  double unitsPerPulse;
  Config(uint32_t m = 100, uint32_t t = 1000000, double u = 1)
      : minIntervalUs(m), timeoutUs(t), unitsPerPulse(u) {}
};
struct Result {
  Status status;
  uint64_t count;
  double ratePerSecond;
};
class PulseMetrics {
  Config c_;
  uint64_t count_;
  uint32_t last_, interval_;

public:
  explicit PulseMetrics(Config c = Config())
      : c_(c), count_(0), last_(0), interval_(0) {}
  bool valid() const {
    return c_.minIntervalUs > 0 && c_.timeoutUs >= c_.minIntervalUs &&
           c_.timeoutUs < 0x80000000UL && detail::finite(c_.unitsPerPulse) &&
           c_.unitsPerPulse > 0;
  }
  Result poll(uint32_t nowUs) const {
    if (!valid())
      return Result{Status::InvalidConfig, count_, 0};
    if (!count_)
      return Result{Status::NoRate, count_, 0};
    const uint32_t age = nowUs - last_;
    if (age >= 0x80000000UL)
      return Result{Status::InvalidTime, count_, 0};
    if (age > c_.timeoutUs)
      return Result{Status::TimedOut, count_, 0};
    if (!interval_)
      return Result{Status::NoRate, count_, 0};
    const double rate = c_.unitsPerPulse * (1000000.0 / interval_);
    return Result{detail::finite(rate) ? Status::Ok : Status::Overflow, count_,
                  detail::finite(rate) ? rate : 0};
  }
  Result pulse(uint32_t atUs) {
    if (!valid())
      return Result{Status::InvalidConfig, count_, 0};
    uint32_t dt = count_ ? atUs - last_ : 0;
    if (count_ && (!dt || dt >= 0x80000000UL))
      return Result{Status::InvalidTime, count_, 0};
    if (count_ && dt < c_.minIntervalUs)
      return Result{Status::Bounce, count_, 0};
    if (count_ == UINT64_MAX)
      return Result{Status::Overflow, count_, 0};
    last_ = atUs;
    ++count_;
    interval_ = dt > c_.timeoutUs ? 0 : dt;
    if (count_ == 1)
      return Result{Status::First, count_, 0};
    return poll(atUs);
  }
  void reset() {
    count_ = 0;
    last_ = interval_ = 0;
  }
};
} // namespace pulse_metrics
} // namespace superjupiter
#endif
