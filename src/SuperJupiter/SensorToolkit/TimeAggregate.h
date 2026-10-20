// Consolidated public release baseline: SuperJupiterTimeAggregate 0.1.1.
#if defined(SUPERJUPITER_TIME_AGGREGATE_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_TIMEAGGREGATE_H
#define SUPERJUPITER_SENSORTOOLKIT_TIMEAGGREGATE_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_TIME_AGGREGATE_VERSION "0.1.1"
namespace superjupiter {
namespace time_aggregate {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
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
  Accepted,
  Completed,
  InvalidTime,
  InvalidConfig,
  NumericError
};
struct Summary {
  uint32_t startMs;
  uint64_t observations, finiteCount, invalidCount;
  double mean, minimum, maximum, coverage;
  bool numericError;
};
class TimeAggregate {
  uint32_t period_, start_, last_, firstFinite_, lastFinite_;
  bool started_, completed_;
  Summary current_, done_;
  uint64_t skipped_;
  void begin(uint32_t atMs) {
    start_ = atMs;
    current_ = Summary{atMs, 0, 0, 0, 0, 0, 0, 0, false};
  }

public:
  explicit TimeAggregate(uint32_t periodMs)
      : period_(periodMs), start_(0), last_(0), firstFinite_(0), lastFinite_(0),
        started_(false), completed_(false), current_(), done_(), skipped_(0) {}
  Summary current() const {
    Summary s = current_;
    s.coverage = s.finiteCount > 1 ? static_cast<double>(static_cast<uint32_t>(
                                         lastFinite_ - firstFinite_)) /
                                         period_
                                   : 0;
    return s;
  }
  bool takeCompleted(Summary &out) {
    if (!completed_)
      return false;
    out = done_;
    completed_ = false;
    return true;
  }
  uint64_t skippedBuckets() const { return skipped_; }
  Status push(double value, uint32_t atMs) {
    if (!period_ || period_ >= 0x80000000UL)
      return Status::InvalidConfig;
    if (started_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return Status::InvalidTime;
    }
    if (!started_) {
      begin(atMs);
      started_ = true;
    }
    const uint32_t elapsed = atMs - start_;
    const uint32_t buckets = elapsed / period_;
    Status status = Status::Accepted;
    if (buckets) {
      done_ = current();
      completed_ = true;
      skipped_ += buckets - 1;
      begin(start_ + buckets * period_);
      status = Status::Completed;
    }
    if (current_.observations == UINT64_MAX)
      return Status::NumericError;
    ++current_.observations;
    last_ = atMs;
    if (!detail::finite(value)) {
      ++current_.invalidCount;
      return status;
    }
    const double n = static_cast<double>(current_.finiteCount + 1);
    const double mean = current_.finiteCount
                            ? detail::blend(current_.mean, value, 1 / n)
                            : value;
    if (!current_.finiteCount) {
      firstFinite_ = atMs;
      current_.minimum = current_.maximum = value;
    } else {
      if (value < current_.minimum)
        current_.minimum = value;
      if (value > current_.maximum)
        current_.maximum = value;
    }
    ++current_.finiteCount;
    lastFinite_ = atMs;
    if (current_.numericError || !detail::finite(mean)) {
      current_.numericError = true;
      current_.mean = 0;
      return Status::NumericError;
    }
    current_.mean = mean;
    return status;
  }
};
} // namespace time_aggregate
} // namespace superjupiter
#endif
