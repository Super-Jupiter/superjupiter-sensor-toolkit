# SuperJupiter ThresholdEvents API

Include `SuperJupiterThresholdEvents.h`. All public types/functions are inside `superjupiter::threshold_events`.

## Behaviour and limits

High alarms enter at >=high and recover at <=low, requiring consecutive qualifying observations spanning the configured duration.
Opposite/intermediate observations cancel a pending transition. Invalid readings explicitly clear certainty to Unknown.
After a gap or invalid reading the next valid input restarts evaluation; no persistence is inferred through missing observations.
Durations and gaps must be below 2^31 ms. No hidden clock, callbacks, latching acknowledgement or safety-controller certification.

## Minimal complete usage

```cpp
#include <SuperJupiterThresholdEvents.h>
using namespace superjupiter::threshold_events;
int main() {
ThresholdEvents alarm(Config(28,30,1000,2000,5000));
alarm.push(31,0);
Result result=alarm.push(31,1000);
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-threshold-events/blob/v0.1.0/src/SuperJupiterThresholdEvents.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Invalid numeric observations clear continuity while preserving monotonic observation time; out-of-order invalid data cannot mutate state.

Numeric invalidity breaks continuity but the observation timestamp is retained. Duplicate or backwards observations, including non-finite ones, are rejected without mutation.
