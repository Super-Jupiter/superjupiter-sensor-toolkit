# SuperJupiter SensorReplay API

Include `SuperJupiterSensorReplay.h`. All public types/functions are inside `superjupiter::sensor_replay`.

## Behaviour and limits

Caller-owned samples have nondecreasing offsets below 2^31 ms and may share timestamps across channels.
Caller arrays must remain alive and unchanged. Faults identify source indices and either drop or replace raw values.
At most one fault per source index; invalid configurations are rejected before start. Raw NaN/Inf values are allowed.
next(nowMs) emits at most one due observation, with its scheduled timestamp, never the polling timestamp.
Polling clocks must advance by less than 2^31 ms from start and never backwards. A run finishes after all source entries are consumed.
No hidden clock, sleep, network, parsing, looping or hardware. Reset/start is explicit and reproducible.

## Minimal complete usage

```cpp
#include <SuperJupiterSensorReplay.h>
using namespace superjupiter::sensor_replay;
int main() {
Sample samples[]={{20,0,0},{21,100,0},{22,200,0}};
SensorReplay replay(samples,3);
replay.start(0);Output reading={};
replay.next(100,reading);
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-sensor-replay/blob/v0.1.0/src/SuperJupiterSensorReplay.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.


## Release 0.1.2

Refresh the exact twenty-component dependency lock to the corrected Integration0.1.2. Hardware validation pending.
