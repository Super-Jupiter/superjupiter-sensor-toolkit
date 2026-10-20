# Validation record

## Release 0.2.0

**Physical hardware validation: pending.** No real board or sensor was executed
for this release. Namespace/build/release improvements do not validate physical
behaviour. No new accuracy, acquisition-rate, memory or runtime claim is made.

Automated checks for this release include the original eight host groups plus
half-range/saturated-age and numeric/combined-flag boundaries, namespace isolation,
GCC/Clang sanitizers, rejection of incompatible fast-math, CMake subproject and
relocated installed consumers, exported package consistency and a PlatformIO
package consumer. Both Arduino examples are compiled for the existing exact
ESP32, ESP32-S3, ESP32-C3 and Uno matrix. Compilation is not execution.

These describe the checked-in release gates. Consult the actual
[v0.2.0 CI](https://github.com/Super-Jupiter/superjupiter-sensor-quality/actions)
before claiming a pass. Release checksum and public registry availability are
verified separately after publication. See [migration](MIGRATION.md) for the
deliberate 0.x API change. No proprietary domain models or customer data added.

## Release 0.1.2

Registry README link and version metadata update only. The monitor and tests are
unchanged from 0.1.0. **Physical hardware validation remains pending.**

## Release 0.1.1

Packaging and registry documentation update only; the monitor and tests are
unchanged from 0.1.0. The PlatformIO export now includes these validation notes,
the API reference and host tests. Consult CI for the exact tagged commit.
**Physical hardware validation remains pending.**

## Release 0.1.0

**Physical hardware validation: pending.** No thermocouple, ADC, ESP32 or other
device was physically tested for this release. No accuracy, field deployment,
energy use or sampling-frequency benchmark is claimed.

The original library was written for public release without importing proprietary
application source, customer data or private Git history.

### Executed software checks

On 19 October 2026 the C++11 host suite passed using GCC with
`-Wall -Wextra -Wconversion -pedantic -Werror` and address/undefined-behaviour
sanitisers. The suite includes eight groups:

1. Initial state, first sample, staleness boundaries and reset.
2. Inclusive bounds, preserved NaN/Inf and invalid-value recovery.
3. Signed rate, exact thresholds, excursions and baseline recovery.
4. Gap thresholds, stale inspection and fresh recovery.
5. Stuck suspicion, tolerance anchor, cumulative drift and interruptions.
6. Duplicate/backward timestamps, no state mutation and 32-bit rollover.
7. Invalid configuration, overflow guards and disabled defaults.
8. A 100,000-observation synthetic stream and independent channels.

There are 200,149 assertions in this run, most belonging to the repeated stream
test; these are not 200,149 independent test cases. One host instance occupied
104 bytes on that compiler/ABI. Embedded sizes must be measured independently.

### Continuous integration

The checked-in workflow runs host tests on GCC and Clang and compiles both
examples for the following exact platforms. Consult the actual run before
claiming a successful build:

| Board | Platform | Evidence type |
| --- | --- | --- |
| ESP32 Dev Module | `esp32:esp32@3.3.12` | Compilation only |
| ESP32-S3 Dev Module | `esp32:esp32@3.3.12` | Compilation only |
| ESP32-C3 Dev Module | `esp32:esp32@3.3.12` | Compilation only |
| Arduino Uno | `arduino:avr@1.8.8` | Compilation only |

Workflow: [CI runs](https://github.com/Super-Jupiter/superjupiter-sensor-quality/actions/workflows/ci.yml).
Passing host tests and compilation do not demonstrate physical behaviour.

## First hardware session

1. Record board/module revision, IDE/core versions and release commit.
2. Run `SyntheticTemperature`; capture raw serial output at 115200 baud.
3. Check expected flags and preserved timestamps against the scenario comments.
4. Integrate an established sensor driver in a separate sketch, with its licence
   and exact hardware/wiring documented. Choose units and physical bounds.
5. Measure fresh conversion cadence rather than just loop/read-call frequency.
6. Observe disconnect, recovery, constant conditions and real excursions.
7. Compare against a reference before making any accuracy claim.
8. Record false positives and threshold tuning, then contribute reproducible
   evidence. Never publish credentials, precise private locations or customer data.

This is a data-quality helper, not a safety-rated control or diagnostic system.


## Release 0.2.1

Hardware validation pending. Independent review regressions run under GCC/Clang sanitizers, source/archive/relocated CMake consumers and the four-board compilation matrix. The exact release commit CI and release receipts are authoritative; a historical failure is not a current passing result. No physical device, sensor or storage power-loss test is claimed.
