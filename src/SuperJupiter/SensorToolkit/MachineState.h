// Consolidated public release baseline: SuperJupiterMachineState 0.1.1.
#if defined(SUPERJUPITER_MACHINE_STATE_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_MACHINESTATE_H
#define SUPERJUPITER_SENSORTOOLKIT_MACHINESTATE_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_MACHINE_STATE_VERSION "0.1.1"
namespace superjupiter {
namespace machine_state {

namespace detail {
inline bool finite(double value) {
  return value == value && value >= -DBL_MAX && value <= DBL_MAX;
}
} // namespace detail

enum class State { Unknown, Stopped, Idle, Running };
enum class Status {
  Ok,
  Gap,
  InvalidInput,
  InvalidTime,
  InvalidConfig,
  Overflow
};
struct Config {
  double stoppedThreshold, runningThreshold;
  uint32_t settleMs, maxGapMs;
  Config(double s = 1, double r = 10, uint32_t settle = 1000,
         uint32_t gap = 10000)
      : stoppedThreshold(s), runningThreshold(r), settleMs(settle),
        maxGapMs(gap) {}
};
struct Result {
  Status status;
  State state;
  bool changed;
};
class MachineState {
  Config c_;
  State state_, candidate_;
  bool timed_;
  uint32_t last_, since_;
  uint64_t durations_[4];

public:
  explicit MachineState(Config c = Config())
      : c_(c), state_(State::Unknown), candidate_(State::Unknown),
        timed_(false), last_(0), since_(0), durations_{} {}
  uint64_t durationMs(State state) const {
    const unsigned i = static_cast<unsigned>(state);
    return i < 4 ? durations_[i] : 0;
  }
  Result push(double activity, uint32_t atMs) {
    if (!detail::finite(c_.stoppedThreshold) ||
        !detail::finite(c_.runningThreshold) ||
        c_.stoppedThreshold >= c_.runningThreshold ||
        c_.settleMs >= 0x80000000UL || !c_.maxGapMs ||
        c_.maxGapMs >= 0x80000000UL)
      return Result{Status::InvalidConfig, state_, false};
    const State before = state_;
    if (timed_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return Result{Status::InvalidTime, state_, false};
    }
    if (!detail::finite(activity)) {
      state_ = candidate_ = State::Unknown;
      timed_ = true;
      last_ = atMs;
      return Result{Status::InvalidInput, state_, state_ != before};
    }
    Status status = Status::Ok;
    if (timed_) {
      const uint32_t dt = atMs - last_;
      if (!dt || dt >= 0x80000000UL)
        return Result{Status::InvalidTime, state_, false};
      if (dt > c_.maxGapMs) {
        state_ = candidate_ = State::Unknown;
        status = Status::Gap;
      } else {
        uint64_t &duration = durations_[static_cast<unsigned>(state_)];
        if (duration > UINT64_MAX - dt)
          return Result{Status::Overflow, state_, false};
        duration += dt;
      }
    }
    timed_ = true;
    last_ = atMs;
    const State wanted =
        activity <= c_.stoppedThreshold
            ? State::Stopped
            : (activity >= c_.runningThreshold ? State::Running : State::Idle);
    if (candidate_ != wanted) {
      candidate_ = wanted;
      since_ = atMs;
    }
    if (static_cast<uint32_t>(atMs - since_) >= c_.settleMs)
      state_ = candidate_;
    return Result{status, state_, before != state_};
  }
};
} // namespace machine_state
} // namespace superjupiter
#endif
