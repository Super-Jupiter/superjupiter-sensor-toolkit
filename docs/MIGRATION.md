# Compatibility and migration

Hardware validation pending. This is preview release 0.1.0. Registry acceptance and indexing are checked separately.

Existing standalone repositories, packages, versions, tags and installations are untouched.
Applications using old includes continue to use the original standalone packages. These new
packages intentionally export no old flat header names and add no dependency on the old packages.

For a migrated component, replace its old include with the path in BASELINE.json. Its component
namespace, API, default settings, result/status types, units and storage bytes remain unchanged.
Older standalone versions retain their own migration requirements; this preview's contract is
with the exact version and public release hash in BASELINE.json, not every historical version.

Installed old and new libraries may coexist. Within ONE application, choose exactly one
implementation of each component, consistently across all translation units. Including the same
component from both distributions in one translation unit is deliberately rejected (an explicit
diagnostic in old-first order, a type/function redefinition in new-first order). Do not mix duplicate
implementations across translation units: include guards cannot detect that; it is unsupported.
The verification suite checks both include orders and rejects duplicate module builds.

No compatibility forwarding package or automatic dependency rewrite is included. Legacy
CMake targets remain provided by legacy packages; new targets have distinct package names.
Explicit dependency/version pins remain recommended. Existing release tags are immutable.

The first consolidation changes packaging and include guards only. Component algorithm bodies
and storage formats are preserved; new bundle version 0.1.0 does not replace component versions.
Any later algorithm or storage-format change needs a separate reviewed change and migration note.
