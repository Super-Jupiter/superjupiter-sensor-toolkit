// Consolidated public release baseline: SuperJupiterSensorReplay 0.1.2.
#if defined(SUPERJUPITER_SENSOR_REPLAY_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_SENSORREPLAY_H
#define SUPERJUPITER_SENSORTOOLKIT_SENSORREPLAY_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_SENSOR_REPLAY_VERSION "0.1.2"
namespace superjupiter {
namespace sensor_replay {

struct Sample {
  double raw;
  uint32_t offsetMs;
  uint16_t channel;
};
enum class FaultKind { Drop, Replace };
struct Fault {
  size_t index;
  FaultKind kind;
  double replacement;
};
struct Output {
  double raw;
  uint32_t atMs;
  uint16_t channel;
  size_t sourceIndex;
};
enum class Status {
  Ready,
  Waiting,
  SampleReady,
  Finished,
  InvalidConfig,
  NotStarted,
  InvalidTime
};
class SensorReplay {
  const Sample *samples_;
  size_t count_;
  const Fault *faults_;
  size_t faultCount_, cursor_;
  uint32_t start_, lastPoll_;
  bool started_, polled_;

public:
  SensorReplay(const Sample *samples, size_t count, const Fault *faults = 0,
               size_t faultCount = 0)
      : samples_(samples), count_(count), faults_(faults),
        faultCount_(faultCount), cursor_(0), start_(0), lastPoll_(0),
        started_(false), polled_(false) {}
  bool valid() const {
    if ((count_ && !samples_) || (faultCount_ && !faults_))
      return false;
    for (size_t i = 0; i < count_; ++i)
      if (samples_[i].offsetMs >= 0x80000000UL ||
          (i && samples_[i].offsetMs < samples_[i - 1].offsetMs))
        return false;
    for (size_t i = 0; i < faultCount_; ++i) {
      if (faults_[i].index >= count_ || (faults_[i].kind != FaultKind::Drop &&
                                         faults_[i].kind != FaultKind::Replace))
        return false;
      for (size_t j = 0; j < i; ++j)
        if (faults_[j].index == faults_[i].index)
          return false;
    }
    return true;
  }
  Status start(uint32_t nowMs) {
    if (!valid())
      return Status::InvalidConfig;
    cursor_ = 0;
    start_ = nowMs;
    started_ = true;
    polled_ = false;
    return Status::Ready;
  }
  size_t consumed() const { return cursor_; }
  Status next(uint32_t nowMs, Output &output) {
    if (!started_)
      return Status::NotStarted;
    const uint32_t elapsed = nowMs - start_;
    if (elapsed >= 0x80000000UL ||
        (polled_ && static_cast<uint32_t>(nowMs - lastPoll_) >= 0x80000000UL))
      return Status::InvalidTime;
    lastPoll_ = nowMs;
    polled_ = true;
    while (cursor_ < count_) {
      const Sample &s = samples_[cursor_];
      if (elapsed < s.offsetMs)
        return Status::Waiting;
      const size_t index = cursor_++;
      bool drop = false;
      double value = s.raw;
      for (size_t i = 0; i < faultCount_; ++i)
        if (faults_[i].index == index) {
          drop = faults_[i].kind == FaultKind::Drop;
          if (!drop)
            value = faults_[i].replacement;
          break;
        }
      if (drop)
        continue;
      output = Output{value, start_ + s.offsetMs, s.channel, index};
      return Status::SampleReady;
    }
    return Status::Finished;
  }
};
} // namespace sensor_replay
} // namespace superjupiter
#endif
