# SuperJupiter PulseMetrics API

Include `SuperJupiterPulseMetrics.h`. All public types/functions are inside `superjupiter::pulse_metrics`.

## Behaviour and limits

Each accepted call represents one pulse; no interrupt handler is installed.
Pulses closer than minIntervalUs are rejected, retaining the last accepted pulse.
Timestamps are uint32 microseconds with consecutive distances below 2^31; timeoutUs is below that horizon.
The reported rate is based on the last accepted interval only, not a window average. The first pulse has no rate.
poll reports stale rate as zero. Count is uint64 and fails explicitly at overflow. Caller handles interrupt synchronisation.

## Minimal complete usage

```cpp
#include <SuperJupiterPulseMetrics.h>
using namespace superjupiter::pulse_metrics;
int main() {
PulseMetrics meter(Config(100,1000000,0.5));
meter.pulse(0);
Result result=meter.pulse(100000);
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-pulse-metrics/blob/v0.1.0/src/SuperJupiterPulseMetrics.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
