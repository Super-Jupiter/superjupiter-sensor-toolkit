// Consolidated public release baseline: SuperJupiterTrend 0.1.1.
#if defined(SUPERJUPITER_TREND_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_TREND_H
#define SUPERJUPITER_SENSORTOOLKIT_TREND_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_TREND_VERSION "0.1.1"
namespace superjupiter {
namespace trend {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
constexpr size_t ringIndex(size_t head, size_t offset, size_t capacity) {
  return offset < capacity - head ? head + offset : offset - (capacity - head);
}
} // namespace detail

enum class Status {
  Ok,
  InvalidInput,
  InvalidTime,
  InvalidConfig,
  Insufficient,
  Gap,
  NumericError
};
struct Config {
  size_t minSamples;
  uint32_t minSpanMs, maxGapMs;
  Config(size_t n = 3, uint32_t span = 1000, uint32_t gap = 10000)
      : minSamples(n), minSpanMs(span), maxGapMs(gap) {}
};
struct Result {
  Status status;
  size_t count;
  uint64_t spanMs;
  double slopePerSecond, intercept;
};
template <size_t Capacity> class Trend {
  static_assert(Capacity >= 2 && Capacity <= 65535,
                "Trend requires capacity 2..65535");
  double values_[Capacity];
  uint32_t intervals_[Capacity];
  size_t head_, count_;
  uint32_t last_;
  Config config_;
  size_t index(size_t i) const { return detail::ringIndex(head_, i, Capacity); }

public:
  explicit Trend(Config config = Config(Capacity < 3 ? Capacity : 3))
      : head_(0), count_(0), last_(0), config_(config) {}
  bool valid() const {
    return !(config_.minSamples < 2 || config_.minSamples > Capacity ||
             config_.maxGapMs == 0 || config_.maxGapMs >= 0x80000000UL);
  }
  Status push(double value, uint32_t atMs) {
    if (!valid())
      return Status::InvalidConfig;
    if (!detail::finite(value))
      return Status::InvalidInput;
    const uint32_t dt = count_ ? atMs - last_ : 0;
    if (count_ && (dt == 0 || dt >= 0x80000000UL))
      return Status::InvalidTime;
    if (count_ == Capacity) {
      head_ = (head_ + 1) % Capacity;
      --count_;
    }
    const size_t i = index(count_);
    values_[i] = value;
    intervals_[i] = dt;
    ++count_;
    last_ = atMs;
    return Status::Ok;
  }
  Result summary() const {
    Result r = {Status::Insufficient, count_, 0, 0, 0};
    if (!valid()) {
      r.status = Status::InvalidConfig;
      return r;
    }
    if (count_ < config_.minSamples)
      return r;
    double meanX = 0, meanY = 0;
    uint64_t elapsed = 0;
    for (size_t i = 0; i < count_; ++i) {
      if (i) {
        const uint32_t dt = intervals_[index(i)];
        if (dt > config_.maxGapMs) {
          r.status = Status::Gap;
          return r;
        }
        elapsed += dt;
      }
      const double n = static_cast<double>(i + 1);
      meanX += (static_cast<double>(elapsed) / 1000 - meanX) / n;
      meanY += (values_[index(i)] - meanY) / n;
    }
    r.spanMs = elapsed;
    if (elapsed < config_.minSpanMs)
      return r;
    double xx = 0, xy = 0;
    elapsed = 0;
    for (size_t i = 0; i < count_; ++i) {
      if (i)
        elapsed += intervals_[index(i)];
      const double dx = static_cast<double>(elapsed) / 1000 - meanX;
      xx += dx * dx;
      xy += dx * (values_[index(i)] - meanY);
    }
    r.slopePerSecond = xy / xx;
    r.intercept = meanY - r.slopePerSecond * meanX;
    if (!detail::finite(xx) || !detail::finite(xy) ||
        !detail::finite(r.slopePerSecond) || !detail::finite(r.intercept) ||
        xx <= 0) {
      r.status = Status::NumericError;
      r.slopePerSecond = 0;
      r.intercept = 0;
      return r;
    }
    r.status = Status::Ok;
    return r;
  }
  void reset() { head_ = count_ = 0; }
};
} // namespace trend
} // namespace superjupiter
#endif
