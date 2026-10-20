// Consolidated public release baseline: SuperJupiterEventCapture 0.1.1.
#if defined(SUPERJUPITER_EVENT_CAPTURE_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_EVENTCAPTURE_H
#define SUPERJUPITER_SENSORTOOLKIT_EVENTCAPTURE_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_EVENT_CAPTURE_VERSION "0.1.1"
namespace superjupiter {
namespace event_capture {

namespace detail {
constexpr size_t ringIndex(size_t head, size_t offset, size_t capacity) {
  return offset < capacity - head ? head + offset : offset - (capacity - head);
}
} // namespace detail

enum class Status { Accepted, Capturing, Complete, InvalidTime, Busy };
struct Sample {
  double raw;
  uint32_t atMs;
};
template <size_t Pre, size_t Post> class EventCapture {
  static_assert(Pre > 0 && Pre <= 65535 && Post > 0 && Post <= 65535 - Pre,
                "EventCapture requires bounded positive capacities");
  Sample history_[Pre], capture_[Pre + Post];
  size_t head_, historyCount_, captured_, remaining_;
  bool triggered_, complete_, timed_;
  uint32_t last_;

public:
  EventCapture()
      : head_(0), historyCount_(0), captured_(0), remaining_(0),
        triggered_(false), complete_(false), timed_(false), last_(0) {}
  Status push(double raw, uint32_t atMs) {
    if (complete_)
      return Status::Complete;
    const uint32_t dt = atMs - last_;
    if (timed_ && (!dt || dt >= 0x80000000UL))
      return Status::InvalidTime;
    last_ = atMs;
    timed_ = true;
    const Sample sample = {raw, atMs};
    if (triggered_) {
      capture_[captured_++] = sample;
      --remaining_;
      if (!remaining_) {
        complete_ = true;
        return Status::Complete;
      }
      return Status::Capturing;
    }
    if (historyCount_ < Pre) {
      history_[detail::ringIndex(head_, historyCount_, Pre)] = sample;
      ++historyCount_;
    } else {
      history_[head_] = sample;
      head_ = (head_ + 1) % Pre;
    }
    return Status::Accepted;
  }
  Status trigger() {
    if (triggered_)
      return Status::Busy;
    for (size_t i = 0; i < historyCount_; ++i)
      capture_[i] = history_[detail::ringIndex(head_, i, Pre)];
    captured_ = historyCount_;
    remaining_ = Post;
    triggered_ = true;
    return Status::Capturing;
  }
  bool complete() const { return complete_; }
  size_t size() const { return captured_; }
  bool get(size_t i, Sample &out) const {
    if (i >= captured_)
      return false;
    out = capture_[i];
    return true;
  }
  void reset() {
    head_ = historyCount_ = captured_ = remaining_ = 0;
    triggered_ = complete_ = timed_ = false;
  }
};
} // namespace event_capture
} // namespace superjupiter
#endif
