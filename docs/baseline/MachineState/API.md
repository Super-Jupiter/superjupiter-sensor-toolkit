# SuperJupiter MachineState API

Include `SuperJupiterMachineState.h`. All public types/functions are inside `superjupiter::machine_state`.

## Behaviour and limits

A scalar activity signal <=stoppedThreshold suggests Stopped, >=runningThreshold suggests Running, otherwise Idle.
A candidate must persist for settleMs across qualifying observations. Changing candidates restarts persistence.
Missing or invalid input changes state to Unknown; no runtime is inferred across a gap exceeding maxGapMs.
Intervals within the gap limit are attributed to the previous confirmed state, including candidate transition time.
Use measured signal-specific thresholds; inferred states are not guaranteed physical truth. Not a machinery safety controller.

## Minimal complete usage

```cpp
#include <SuperJupiterMachineState.h>
using namespace superjupiter::machine_state;
int main() {
MachineState machine(Config(2,50,1000,5000));
machine.push(100,0);
Result state=machine.push(100,1000);
(void)state;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-machine-state/blob/v0.1.0/src/SuperJupiterMachineState.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Invalid numeric observations clear continuity while preserving monotonic observation time; out-of-order invalid data cannot mutate state.

Numeric invalidity breaks continuity but the observation timestamp is retained. Duplicate or backwards observations, including non-finite ones, are rejected without mutation.
