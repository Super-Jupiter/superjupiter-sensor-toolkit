// Consolidated public release baseline: SuperJupiterCalibration 0.1.1.
#if defined(SUPERJUPITER_CALIBRATION_H)
#error "Duplicate SuperJupiter module: use legacy OR consolidated headers for this component in one application"
#endif
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_SENSORTOOLKIT_CALIBRATION_H
#define SUPERJUPITER_SENSORTOOLKIT_CALIBRATION_H
#if defined(__FAST_MATH__) ||                                                  \
    (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#define SUPERJUPITER_CALIBRATION_VERSION "0.1.1"
namespace superjupiter {
namespace calibration {

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

enum class Bounds { Reject, Clamp, Extrapolate };
enum class Status {
  Ok,
  Clamped,
  Extrapolated,
  InvalidTable,
  InvalidInput,
  OutOfRange,
  NumericError
};
struct Point {
  double input;
  double output;
};
struct Result {
  Status status;
  double raw;
  double value;
  uint32_t revision;
  bool valid() const {
    return status == Status::Ok || status == Status::Clamped ||
           status == Status::Extrapolated;
  }
};
class Calibration {
  const Point *points_;
  size_t count_;
  uint32_t revision_;
  Bounds bounds_;

public:
  Calibration(const Point *points, size_t count, uint32_t revision = 0,
              Bounds bounds = Bounds::Reject)
      : points_(points), count_(count), revision_(revision), bounds_(bounds) {}
  bool valid() const {
    if (!points_ || count_ < 2 ||
        (bounds_ != Bounds::Reject && bounds_ != Bounds::Clamp &&
         bounds_ != Bounds::Extrapolate))
      return false;
    for (size_t i = 0; i < count_; ++i)
      if (!detail::finite(points_[i].input) ||
          !detail::finite(points_[i].output) ||
          (i && points_[i].input <= points_[i - 1].input))
        return false;
    return true;
  }
  Result evaluate(double raw) const {
    Result r = {Status::InvalidTable, raw, 0, revision_};
    if (!valid())
      return r;
    if (!detail::finite(raw)) {
      r.status = Status::InvalidInput;
      return r;
    }
    const bool outside =
        raw < points_[0].input || raw > points_[count_ - 1].input;
    if (outside && bounds_ == Bounds::Reject) {
      r.status = Status::OutOfRange;
      return r;
    }
    if (outside && bounds_ == Bounds::Clamp) {
      r.value = raw < points_[0].input ? points_[0].output
                                       : points_[count_ - 1].output;
      r.status = Status::Clamped;
      return r;
    }
    size_t upper = 1;
    while (upper < count_ - 1 && raw > points_[upper].input)
      ++upper;
    const Point &a = points_[upper - 1];
    const Point &b = points_[upper];
    // Exact knots preserve endpoint values, including signed zero, without
    // risky arithmetic.
    if (raw == a.input) {
      r.value = a.output;
      r.status = outside ? Status::Extrapolated : Status::Ok;
      return r;
    }
    if (raw == b.input) {
      r.value = b.output;
      r.status = Status::Ok;
      return r;
    }
    if (a.output == b.output) {
      r.value = a.output;
      r.status = outside ? Status::Extrapolated : Status::Ok;
      return r;
    }
    double span = b.input - a.input, numerator = raw - a.input;
    if (!detail::finite(span) || !detail::finite(numerator)) {
      span = b.input / 2 - a.input / 2;
      numerator = raw / 2 - a.input / 2;
    }
    const double fraction = numerator / span;
    r.value = outside ? (fraction > 1
                             ? b.output + (fraction - 1) * (b.output - a.output)
                             : a.output + fraction * (b.output - a.output))
                      : detail::blend(a.output, b.output, fraction);
    if (!detail::finite(span) || span <= 0 || !detail::finite(fraction) ||
        !detail::finite(r.value)) {
      r.status = Status::NumericError;
      r.value = 0;
      return r;
    }
    r.status = outside ? Status::Extrapolated : Status::Ok;
    return r;
  }
};
} // namespace calibration
} // namespace superjupiter
#endif
