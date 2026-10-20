// Consolidated public release baseline: SuperJupiterIntegration 0.1.2.
#if defined(SUPERJUPITER_INTEGRATION_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_INTEGRATION_H
#define SUPERJUPITER_SENSORTOOLKIT_INTEGRATION_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_INTEGRATION_VERSION "0.1.2"
namespace superjupiter {
namespace integration {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
} // namespace detail

enum class Method { Trapezoid, Previous };
enum class Status {
  Seeded,
  Integrated,
  GapSkipped,
  InvalidInput,
  InvalidTime,
  InvalidConfig,
  NumericError
};
struct Result {
  Status status;
  double valueSeconds;
  uint64_t coveredMs, skippedMs;
};
class Integration {
  uint32_t gap_, last_;
  Method method_;
  bool seeded_, timed_;
  double previous_, total_;
  uint64_t covered_, skipped_;
  Result result(Status s) const {
    return Result{s, total_, covered_, skipped_};
  }

public:
  explicit Integration(uint32_t maxGapMs = 10000,
                       Method method = Method::Trapezoid)
      : gap_(maxGapMs), last_(0), method_(method), seeded_(false),
        timed_(false), previous_(0), total_(0), covered_(0), skipped_(0) {}
  Result push(double value, uint32_t atMs) {
    if (!gap_ || gap_ >= 0x80000000UL ||
        (method_ != Method::Trapezoid && method_ != Method::Previous))
      return result(Status::InvalidConfig);
    if (timed_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return result(Status::InvalidTime);
    }
    if (!detail::finite(value)) {
      seeded_ = false;
      timed_ = true;
      last_ = atMs;
      return result(Status::InvalidInput);
    }
    if (!seeded_) {
      seeded_ = true;
      timed_ = true;
      previous_ = value;
      last_ = atMs;
      return result(Status::Seeded);
    }
    const uint32_t dt = atMs - last_;
    if (!dt || dt >= 0x80000000UL)
      return result(Status::InvalidTime);
    if (dt > gap_) {
      if (skipped_ > UINT64_MAX - dt)
        return result(Status::NumericError);
      skipped_ += dt;
      previous_ = value;
      last_ = atMs;
      return result(Status::GapSkipped);
    }
    // Sum first when safe, preserving subnormals. Halve first only near
    // overflow, where loss of a tiny half cannot affect the rounded result.
    const double halfMax = DBL_MAX / 2;
    const bool safeSum = previous_ >= -halfMax && previous_ <= halfMax &&
                         value >= -halfMax && value <= halfMax;
    const double average =
        method_ == Method::Previous
            ? previous_
            : (safeSum ? (previous_ + value) / 2 : previous_ / 2 + value / 2);
    const double area = average * (static_cast<double>(dt) / 1000);
    const double next = total_ + area;
    if (!detail::finite(area) || !detail::finite(next) ||
        covered_ > UINT64_MAX - dt)
      return result(Status::NumericError);
    total_ = next;
    covered_ += dt;
    previous_ = value;
    last_ = atMs;
    return result(Status::Integrated);
  }
  Result summary() const { return result(Status::Integrated); }
  void reset() {
    seeded_ = false;
    timed_ = false;
    last_ = 0;
    previous_ = total_ = 0;
    covered_ = skipped_ = 0;
  }
};
} // namespace integration
} // namespace superjupiter
#endif
