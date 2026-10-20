# SuperJupiter Calibration API

Include `SuperJupiterCalibration.h`. All public types/functions are inside `superjupiter::calibration`.

## Behaviour and limits

Caller-owned, strictly increasing input knots, finite outputs and at least two points are required.
Tables must remain alive and unchanged while used. Evaluate never alters the raw input.
Outside-table policy is reject, clamp or linear extrapolation. Results report which policy applied.
Calibration revision is caller-assigned provenance, not a certificate of physical accuracy.
O(N) validation/evaluation, no allocation. Input/output units are caller-defined and must be documented.

## Minimal complete usage

```cpp
#include <SuperJupiterCalibration.h>
using namespace superjupiter::calibration;
int main() {
Point points[]={{0,0},{4095,100}};
Calibration calibration(points,2,1);
Result result=calibration.evaluate(2047.5);
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-calibration/blob/v0.1.0/src/SuperJupiterCalibration.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Exact knots and finite wide-range interpolation work without intermediate overflow; invalid bounds rejected.
