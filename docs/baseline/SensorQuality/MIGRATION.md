# Migrating from 0.1.x to 0.2.0

0.2.0 deliberately moves the public C++ types and flags into a component namespace
before this early library becomes a template for other SuperJupiter libraries.
The package name, repository, include header and monitor behaviour are unchanged.
Existing 0.1.x releases and tags remain available and are not overwritten.

```cpp
// 0.1.x
superjupiter::Config config;
superjupiter::SensorQuality monitor(config);

// 0.2.0
superjupiter::sensor_quality::Config config;
superjupiter::sensor_quality::SensorQuality monitor(config);
```

Change `Result`, `Flag`, `flagName` and enumerator references in the same way:
`superjupiter::Stale` becomes `superjupiter::sensor_quality::Stale`.
An application may use `namespace quality = superjupiter::sensor_quality;` for
shorter local references. Public package branding remains SuperJupiter.

No root namespace compatibility aliases are added: they would retain the generic
`Config`, `Result` and `Flag` collisions this migration is intended to prevent.
Pin 0.1.2 until your application has been migrated and rebuilt if you cannot make
this source change yet. Test your application rather than assuming a 0.x update
has no API changes. A future breaking change will get another documented release.

For CMake, link the exported `SuperJupiter::SensorQuality` target. The old local
`SuperJupiterSensorQuality` target still exists in an add_subdirectory build.
Developer executables now have the `superjupiter-sensor-quality-` prefix. Tests,
examples and installation default to OFF for subproject builds and ON at top level.

GCC/Clang fast-math and finite-math-only configurations now fail compilation with
a clear diagnostic. Their assumptions are incompatible with invalid-number
detection. Other compilers must also preserve non-finite values; this release
does not claim to detect every compiler's incompatible option.

Physical hardware validation remains pending.
