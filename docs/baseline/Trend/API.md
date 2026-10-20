# SuperJupiter Trend API

Include `SuperJupiterTrend.h`. All public types/functions are inside `superjupiter::trend`.

## Behaviour and limits

Fixed-capacity last-N finite samples; rejected inputs do not evict history.
Timestamps must advance by 1..2^31-1 milliseconds. Regression uses elapsed seconds relative to the oldest sample.
summary requires minSamples, minSpanMs and no retained adjacent interval above maxGapMs.
Slope is value units per second; intercept is the fitted value at the oldest retained sample.
This is descriptive linear regression, not a forecast or statistical confidence estimate. O(N) summary.

## Minimal complete usage

```cpp
#include <SuperJupiterTrend.h>
using namespace superjupiter::trend;
int main() {
Trend<8> trend(Config(3,200,1000));
trend.push(20,0);trend.push(21,100);trend.push(22,200);
Result result=trend.summary();
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-trend/blob/v0.1.0/src/SuperJupiterTrend.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Capacity-two default works; summary reports invalid config; ring indexing avoids size_t overflow.
