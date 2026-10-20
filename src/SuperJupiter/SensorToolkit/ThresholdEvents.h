// Consolidated public release baseline: SuperJupiterThresholdEvents 0.1.1.
#if defined(SUPERJUPITER_THRESHOLD_EVENTS_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_THRESHOLDEVENTS_H
#define SUPERJUPITER_SENSORTOOLKIT_THRESHOLDEVENTS_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_THRESHOLD_EVENTS_VERSION "0.1.1"
namespace superjupiter {
namespace threshold_events {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
} // namespace detail

enum class State { Unknown, Normal, Alarm };
enum class Status { Ok, InvalidInput, InvalidTime, InvalidConfig, GapReset };
struct Config {
  double low, high;
  uint32_t enterMs, recoverMs, maxGapMs;
  Config(double l = 0, double h = 1, uint32_t e = 0, uint32_t r = 0,
         uint32_t g = 10000)
      : low(l), high(h), enterMs(e), recoverMs(r), maxGapMs(g) {}
};
struct Result {
  Status status;
  State state;
  bool changed;
};
class ThresholdEvents {
  Config c_;
  State state_;
  bool timed_, pending_;
  uint32_t last_, since_;
  State target_;

public:
  explicit ThresholdEvents(Config c = Config())
      : c_(c), state_(State::Unknown), timed_(false), pending_(false), last_(0),
        since_(0), target_(State::Unknown) {}
  Result push(double value, uint32_t atMs) {
    if (!detail::finite(c_.low) || !detail::finite(c_.high) ||
        c_.low >= c_.high || c_.enterMs >= 0x80000000UL ||
        c_.recoverMs >= 0x80000000UL || !c_.maxGapMs ||
        c_.maxGapMs >= 0x80000000UL)
      return Result{Status::InvalidConfig, state_, false};
    const State before = state_;
    if (timed_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return Result{Status::InvalidTime, state_, false};
    }
    if (!detail::finite(value)) {
      state_ = State::Unknown;
      timed_ = true;
      last_ = atMs;
      pending_ = false;
      return Result{Status::InvalidInput, state_, state_ != before};
    }
    Status status = Status::Ok;
    if (timed_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return Result{Status::InvalidTime, state_, false};
      if (dt > c_.maxGapMs) {
        state_ = State::Unknown;
        pending_ = false;
        status = Status::GapReset;
      }
    }
    timed_ = true;
    last_ = atMs;
    State desired = state_;
    if (value >= c_.high)
      desired = State::Alarm;
    else if (value <= c_.low)
      desired = State::Normal;
    if (desired == state_)
      pending_ = false;
    else {
      if (!pending_ || target_ != desired) {
        pending_ = true;
        target_ = desired;
        since_ = atMs;
      }
      const uint32_t duration =
          desired == State::Alarm ? c_.enterMs : c_.recoverMs;
      if (static_cast<uint32_t>(atMs - since_) >= duration) {
        state_ = desired;
        pending_ = false;
      }
    }
    return Result{status, state_, state_ != before};
  }
  State state() const { return state_; }
};
} // namespace threshold_events
} // namespace superjupiter
#endif
