# Validation: preview release 0.1.0

**Hardware validation pending.**

Local verification completed 2026-10-20T13:15:47.354272+00:00. Exact public release provenance is in BASELINE.json.
Component bodies match the original public release headers after reversing only guard plumbing.
Existing standalone repositories, installs, tags, APIs and storage byte formats are preserved.

## Executed checks

- GCC and Clang: 160 legacy/consolidated component regression and consumer executables each,
  with strict C++11 warnings and AddressSanitizer/UndefinedBehaviorSanitizer.
- 40 duplicate-module include-order rejection cases per compiler; unsafe-math rejection checks.
- Optional all-20 synthetic pipeline executed on both host compilers.
- Journal/configuration persisted bytes compared against the original standalone versions.
- Both new CMake packages consumed from source and after installation and relocation.
- 80 CMake/CTest cases passed from isolated local ZIP extractions (60 Toolkit, 20 Reliability).
- Nine PlatformIO Arduino-framework build environments passed: Uno small examples and legacy
  comparison, ESP32/ESP32-S3/ESP32-C3 combined pipeline, and clean local-archive consumers.
- Small Uno quality/history comparison: legacy and consolidated builds both 6134 flash bytes
  and 351 static RAM bytes. This is one fixture, not a guarantee for other capacities or boards.

Board toolchains: PlatformIO Core 6.1.19, atmelavr 5.1.0 (Arduino AVR package 5.2.0),
espressif32 6.12.0 (Arduino ESP32 2.0.17). Package versions are recorded separately from
board compilation and hardware testing. Initial package-name/path issues in the local test harness
were repaired before the final passing runs; component algorithms did not change.

## Limits

Board compilation is not hardware execution. No physical sensors, storage power-cut tests,
network outage tests or field measurements were performed. Durability depends on storage adapters.
No Arduino IDE/Arduino CLI installation test is claimed: ZIP structure/content was audited and
its extracted CMake tests executed; board consumers used PlatformIO's Arduino frameworks.
Mixing old and new implementations of the same component across translation units is unsupported.
The baseline contract covers the listed component releases, not every historical package version.
