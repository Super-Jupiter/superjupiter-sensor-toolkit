// Consolidated public release baseline: SuperJupiterSignalFilter 0.1.1.
#if defined(SUPERJUPITER_SIGNAL_FILTER_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_SIGNALFILTER_H
#define SUPERJUPITER_SENSORTOOLKIT_SIGNALFILTER_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_SIGNAL_FILTER_VERSION "0.1.1"
namespace superjupiter {
namespace signal_filter {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
inline double oneMinusExpNegative(double ratio) {
  // Stable at small elapsed/tau without requiring AVR's unavailable expm1.
  if (ratio < 0.01) {
    double term = ratio, total = ratio;
    for (unsigned n = 2; n <= 12; ++n) {
      term *= -ratio / n;
      total += term;
    }
    return total;
  }
  return 1 - ::exp(-ratio);
}
inline double blend(double a, double b, double fraction) {
  if (fraction == 0 || a == b)
    return a;
  if (fraction == 1)
    return b;
  return (a < 0) != (b < 0) ? (1 - fraction) * a + fraction * b
                            : a + fraction * (b - a);
}
} // namespace detail

enum class Status {
  Seeded,
  Filtered,
  GapReset,
  InvalidConfig,
  InvalidInput,
  InvalidTime,
  NumericError
};
struct Config {
  double tauMs;
  uint32_t maxGapMs;
  Config(double tau = 1000, uint32_t gap = 10000) : tauMs(tau), maxGapMs(gap) {}
};
struct Result {
  Status status;
  double value;
};
class SignalFilter {
  Config config_;
  bool seeded_;
  double value_;
  uint32_t time_;

public:
  explicit SignalFilter(Config config = Config())
      : config_(config), seeded_(false), value_(0), time_(0) {}
  void reset() {
    seeded_ = false;
    value_ = 0;
    time_ = 0;
  }
  Result push(double raw, uint32_t atMs) {
    if (!detail::finite(config_.tauMs) || config_.tauMs <= 0 ||
        config_.maxGapMs == 0 || config_.maxGapMs >= 0x80000000UL)
      return Result{Status::InvalidConfig, value_};
    if (!detail::finite(raw))
      return Result{Status::InvalidInput, value_};
    if (!seeded_) {
      seeded_ = true;
      value_ = raw;
      time_ = atMs;
      return Result{Status::Seeded, value_};
    }
    const uint32_t dt = atMs - time_;
    if (dt == 0 || dt >= 0x80000000UL)
      return Result{Status::InvalidTime, value_};
    if (dt > config_.maxGapMs) {
      value_ = raw;
      time_ = atMs;
      return Result{Status::GapReset, value_};
    }
    const double alpha =
        detail::oneMinusExpNegative(static_cast<double>(dt) / config_.tauMs);
    const double next = detail::blend(value_, raw, alpha);
    if (!detail::finite(next))
      return Result{Status::NumericError, value_};
    value_ = next;
    time_ = atMs;
    return Result{Status::Filtered, value_};
  }
};
} // namespace signal_filter
} // namespace superjupiter
#endif
