# SuperJupiter Integration API

Include `SuperJupiterIntegration.h`. All public types/functions are inside `superjupiter::integration`.

## Behaviour and limits

Integrates in value-seconds using trapezoidal or previous-value rectangular intervals.
Invalid input breaks the integration chain; the next valid input seeds a new segment.
Intervals above maxGapMs are explicitly skipped, never extrapolated; skipped duration is reported.
Caller timestamps advance by 1..2^31-1 ms. Integral may be signed. Coverage counts only accepted integrated intervals.
No physical unit conversion, interpolation across missing data, reset persistence or meter certification.

## Minimal complete usage

```cpp
#include <SuperJupiterIntegration.h>
using namespace superjupiter::integration;
int main() {
Integration energy(5000);
energy.push(100,0);
Result joules=energy.push(100,1000);
(void)joules;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-integration/blob/v0.1.0/src/SuperJupiterIntegration.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Chronology survives invalid inputs without integrating across them; invalid integration methods rejected.

Numeric invalidity breaks continuity but the observation timestamp is retained. Duplicate or backwards observations, including non-finite ones, are rejected without mutation.


## Release 0.1.2

Preserve representable subnormal integrals while retaining overflow-safe trapezoid averages. Hardware validation pending.
