# SuperJupiter SensorAgreement API

Include `SuperJupiterSensorAgreement.h`. All public types/functions are inside `superjupiter::sensor_agreement`.

## Behaviour and limits

Caller supplies values and ages for simultaneous comparison in common physical units.
Non-finite or stale samples are excluded; at least minSensors eligible values are required.
Reports mean, spread, eligible/excluded counts, and agreement against absoluteTolerance + relativeTolerance*max(abs(values)).
Agreement does not establish accuracy or identify the faulty sensor. No majority voting, automatic substitution or sensor health certification.
Bounded by caller-provided input length; no allocation, state or retained pointers.

## Minimal complete usage

```cpp
#include <SuperJupiterSensorAgreement.h>
using namespace superjupiter::sensor_agreement;
int main() {
Sample sensors[]={{20,0},{20.2,100},{19.9,50}};
Result agreement=compare(sensors,3,Config(0.5,0,1000,2));
(void)agreement;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-sensor-agreement/blob/v0.1.0/src/SuperJupiterSensorAgreement.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Equal readings preserve their exact common value; stable incremental mean.
