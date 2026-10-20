# API and semantics

Since 0.2.0, types and flags are in the component namespace
`superjupiter::sensor_quality`; see [MIGRATION.md](MIGRATION.md).

Include `SuperJupiterSensorQuality.h`. All types are in namespace `superjupiter::sensor_quality`.
One `SensorQuality` instance monitors one scalar channel. Values use `float`;
rates use `double`, whose precision depends on the target architecture.

## Configuration

Pass a `Config` at construction. To change settings construct another monitor.
`reset()` clears all history but retains the settings.

| Field | Default | Rule |
| --- | --- | --- |
| `rangeEnabled` | `false` | Enables inclusive bounds. |
| `minimum`, `maximum` | `0`, `100` | When enabled, both must be finite and minimum <= maximum. |
| `maxRatePerSecond` | `0` | Nonnegative finite units/second. Zero disables checking. |
| `expectedIntervalMs` | `0` | Zero disables late-sample checking. |
| `gapToleranceMs` | `0` | Must be zero if expected interval is zero. |
| `staleAfterMs` | `0` | Zero disables staleness checking. |
| `stuckAfterMs` | `0` | Zero disables stuck checking. |
| `stuckTolerance` | `0` | Nonnegative finite absolute distance from the run's anchor. |

Time limits and expected interval plus tolerance must be <= `0x7fffffff` ms.
`SensorQuality::validConfig(config)` checks configuration without observing a
reading. `configured()` reports the constructed monitor's configuration status.
An invalid configuration disables observation updates and reports
`InvalidConfiguration`; it is never silently corrected.

## `observe(float value, uint32_t sampledAtMs)`

Submit a fresh scalar observation and its capture time. The value is preserved.
This function does not read a sensor, filter a signal or read a clock.

- The first timestamp may be zero. Subsequent timestamps must advance.
- Duplicate or out-of-order timestamps return `InvalidTimestamp`, without
  modifying history. Do not persist that returned attempt as an accepted sample
  without also preserving the error flag.
- NaN and infinity are `InvalidNumber`. Bounds are inclusive.
- Any finite, in-range observation refreshes the staleness timestamp, including
  one flagged `RateExceeded` or `Stuck`. Freshness and plausibility are separate.
- Invalid and out-of-range observations do not refresh that timestamp. They
  break the rate and stuck run, so the next usable reading starts a new baseline.
- A sample gap is strictly **greater than** expected interval plus tolerance.
  `SampleGap` means a late observation. With timestamps alone it is impossible
  to distinguish scheduling delay from actual missing measurements.
- A gap, or an observation interval meeting `staleAfterMs`, breaks continuity
  for rate and stuck checks. No local rate is inferred across that interruption.
- Rate uses actual elapsed time between consecutive usable observations. The
  absolute rate must be strictly greater than the limit to be flagged. The
  returned signed rate retains direction. If floating arithmetic overflows,
  `hasRate` is false and an enabled rate check reports `RateExceeded`.
- A rate excursion updates the next rate baseline. An isolated spike can flag
  both its rising and falling edges; a lasting step can recover on the next
  stable sample.
- Stuck detection compares every value with the run's first value. It does not
  compare only successive differences, which could hide slow cumulative drift.
  The flag begins at age >= `stuckAfterMs` and clears when the signal leaves the
  anchor tolerance or continuity is broken. It is evidence of sameness only,
  never proof of hardware failure.

The result is a **snapshot**, not a cumulative event register. Applications that
need counts or event history must store returned flags alongside raw samples.

## `inspect(uint32_t nowMs) const`

Returns the most recent observation snapshot and adds current staleness or a
timestamp error. It does not mutate state, refresh a sample, or create a new
timestamp. Call it periodically while waiting for a driver that may stop replying.

Before any observation, `NoData` is returned (`hasReading == false`), without
`Stale`. Following an invalid observation with no usable baseline, an enabled
staleness check returns `Stale` immediately. After a usable observation, the
age is compared to the configured threshold with **>=**.

The last observation's flags remain in this result. For example, an old `SampleGap`
does not disappear merely because `inspect()` was called. A new observation
replaces that snapshot. `inspect()` does not newly infer `Stuck` during silence;
silence is a freshness problem, not proof of a constant measured signal.

## Result

`flags` is a bitmask. Use `has(superjupiter::sensor_quality::Stale)` to test an individual flag.
`healthy()` means no configured quality check raised a flag; it is not an accuracy
guarantee. `hasReading` distinguishes a real observation from the default zero.
`hasRate` distinguishes a calculated rate from the default zero rate.

`sampledAtMs` is capture time. `intervalMs` is the interval between submitted
observations, including invalid ones. It is not an ADC conversion interval.
`ratePerSecond` uses consecutive finite, in-range readings when continuity holds.
Use `flagName()` for one flag at a time; it does not format a combined bitmask.

## Clock, precision and concurrency limits

Unsigned subtraction supports the usual 32-bit `millis()` rollover. Consecutive
observations must be no more than `0x7fffffff` milliseconds apart (about 24.9 days).
An `inspect()` timestamp must be within that span after the last observation.
Long-lived stuck and stale durations saturate instead of wrapping, so repeated
clock rollovers do not clear established flags when observations continue within
the supported interval. Reset after reboot, clock restart or switching clock
sources. Do not mix wall-clock time with `millis()`. Use one consistent millisecond
clock and chronological calls.

The library does not detect a sensor driver returning cached readings as fresh,
or prove the physical conversion rate. Float quantisation can make a slowly
moving value appear constant. Tune thresholds to the actual signal, units,
resolution and expected dynamics.

There is no locking. Call from one execution context, or synchronise externally.
The API makes no ISR-safety or hard-real-time guarantees. It has no heap
allocation and stores bounded state; it does not store a sample history.

Compile with normal floating-point semantics. Fast-math/finite-math-only options
are unsupported because they may assume NaN and infinity cannot occur. Known
GCC/Clang macros for these modes are rejected by the public header. This is not
a guarantee that every compiler's incompatible option is detectable.


## 0.2.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Explicit numeric-error flag and zero unavailable rate for reduced-precision rate overflow; raw readings retained.
