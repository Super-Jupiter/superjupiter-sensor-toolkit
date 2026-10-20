// Consolidated public release baseline: SuperJupiterTimeAnchor 0.1.1.
#if defined(SUPERJUPITER_TIME_ANCHOR_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_TIMEANCHOR_H
#define SUPERJUPITER_SENSORTOOLKIT_TIMEANCHOR_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_TIME_ANCHOR_VERSION "0.1.1"
namespace superjupiter {
namespace time_anchor {

enum class Status {
  Ok,
  NoAnchor,
  InvalidConfig,
  InvalidAnchor,
  OutsideHorizon,
  Expired,
  Overflow
};
struct Config {
  uint32_t maxAgeMs;
  uint32_t driftPpm;
  Config(uint32_t age = 86400000, uint32_t drift = 100)
      : maxAgeMs(age), driftPpm(drift) {}
};
struct Result {
  Status status;
  int64_t utcMs;
  uint64_t uncertaintyMs;
  uint32_t revision;
};
class TimeAnchor {
  Config config_;
  bool anchored_;
  uint32_t tick_, revision_, uncertainty_;
  int64_t utc_;

public:
  explicit TimeAnchor(Config config = Config())
      : config_(config), anchored_(false), tick_(0), revision_(0),
        uncertainty_(0), utc_(0) {}
  Status setAnchor(uint32_t tickMs, int64_t utcMs, uint32_t uncertaintyMs) {
    if (config_.maxAgeMs >= 0x80000000UL)
      return Status::InvalidConfig;
    if (utcMs < 0 || revision_ == UINT32_MAX)
      return Status::InvalidAnchor;
    tick_ = tickMs;
    utc_ = utcMs;
    uncertainty_ = uncertaintyMs;
    anchored_ = true;
    ++revision_;
    return Status::Ok;
  }
  Result convert(uint32_t tickMs) const {
    Result r = {Status::NoAnchor, 0, 0, revision_};
    if (!anchored_)
      return r;
    const uint32_t dt = tickMs - tick_;
    if (dt >= 0x80000000UL) {
      r.status = Status::OutsideHorizon;
      return r;
    }
    if (dt > config_.maxAgeMs) {
      r.status = Status::Expired;
      return r;
    }
    if (utc_ > INT64_MAX - static_cast<int64_t>(dt)) {
      r.status = Status::Overflow;
      return r;
    }
    r.utcMs = utc_ + dt;
    r.uncertaintyMs =
        uncertainty_ +
        (static_cast<uint64_t>(dt) * config_.driftPpm + 999999) / 1000000;
    r.status = Status::Ok;
    return r;
  }
  void clear() { anchored_ = false; }
};
} // namespace time_anchor
} // namespace superjupiter
#endif
