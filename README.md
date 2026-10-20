# SuperJupiter Sensor Toolkit

**Preview release 0.1.0. Hardware validation pending.**

Check readings, preserve timestamped history and process sensor observations. This is not a hardware driver or physical sensor certification.

Original public MIT components by [SuperJupiter](https://www.superjupiter.net).
Independent C++11 package; no mandatory dependency on the other consolidated library, no service,
no automatic network traffic and no automatically started tasks. Select individual module headers.
The convenience header excludes testing/tools-only modules. Resource use depends on selected
components and capacities; no claim of zero flash or RAM cost is made.

## Modules

| Module | Include | Public baseline version |
| --- | --- | --- |
| SensorQuality | `SuperJupiter/SensorToolkit/SensorQuality.h` | 0.2.1 |
| SampleWindow | `SuperJupiter/SensorToolkit/SampleWindow.h` | 0.1.1 |
| Calibration | `SuperJupiter/SensorToolkit/Calibration.h` | 0.1.1 |
| TimeAnchor | `SuperJupiter/SensorToolkit/TimeAnchor.h` | 0.1.1 |
| SignalFilter | `SuperJupiter/SensorToolkit/SignalFilter.h` | 0.1.1 |
| Trend | `SuperJupiter/SensorToolkit/Trend.h` | 0.1.1 |
| ThresholdEvents | `SuperJupiter/SensorToolkit/ThresholdEvents.h` | 0.1.1 |
| PulseMetrics | `SuperJupiter/SensorToolkit/PulseMetrics.h` | 0.1.1 |
| TimeAggregate | `SuperJupiter/SensorToolkit/TimeAggregate.h` | 0.1.1 |
| Integration | `SuperJupiter/SensorToolkit/Integration.h` | 0.1.2 |
| EventCapture | `SuperJupiter/SensorToolkit/EventCapture.h` | 0.1.1 |
| ChannelAlign | `SuperJupiter/SensorToolkit/ChannelAlign.h` | 0.1.1 |
| SensorAgreement | `SuperJupiter/SensorToolkit/SensorAgreement.h` | 0.1.1 |
| MachineState | `SuperJupiter/SensorToolkit/MachineState.h` | 0.1.1 |
| SensorReplay | `SuperJupiter/SensorToolkit/testing/SensorReplay.h` | 0.1.2 |

## Installation

Arduino: download the versioned ZIP from the [0.1.0 release](https://github.com/Super-Jupiter/superjupiter-sensor-toolkit/releases/tag/v0.1.0) and use Sketch > Include Library > Add .ZIP Library.
Library Manager registration and index propagation are separate; check the official index before relying on IDE availability.

PlatformIO exact version:

```ini
lib_deps = superjupiter/SuperJupiterSensorToolkit@0.1.0
```

For manual installation or offline builds, use the versioned GitHub release assets.

```cmake
add_subdirectory(path/to/superjupiter-sensor-toolkit)
target_link_libraries(your_app PRIVATE SuperJupiter::SensorToolkit)
```

After CMake installation: `find_package(SuperJupiterSensorToolkit 0.1 CONFIG REQUIRED)`.
Tests default off in add_subdirectory consumers. No global compiler flags are changed.

## Examples and limits

The examples directory contains module examples adapted from the exact public releases.
Examples use synthetic readings or RAM storage unless explicitly stated. Outcomes must be checked
before using results. Preserve raw data. No physical measurements, safety guarantees or field
durability are claimed. Component documentation is retained under docs/baseline; its versioned
references describe the source component, not a separately installed dependency of this bundle.

See [migration](docs/MIGRATION.md), [validation](docs/VALIDATION.md), and BASELINE.json for provenance.
Do not combine legacy and consolidated implementations of the same component in one application.
Public examples exclude customer information, proprietary products and private configurations.

## Support

Report reproducible issues through [GitHub issues](https://github.com/Super-Jupiter/superjupiter-sensor-toolkit/issues). For private security reports contact info@superjupiter.net. Never include credentials or client data.

## Verify locally

```sh
python3 tools/verify.py
python3 tools/verify.py --test --compiler g++
```

Start with `examples/CheckReadings`. All hardware validation remains pending.
