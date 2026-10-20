# SuperJupiter SignalFilter API

Include `SuperJupiterSignalFilter.h`. All public types/functions are inside `superjupiter::signal_filter`.

## Behaviour and limits

The filter uses alpha=-expm1(-elapsed/tau) for positive finite tauMs.
First valid observation seeds the state; a gap beyond maxGapMs reseeds rather than bridging missing data.
Duplicate/backward timestamps and invalid numeric inputs leave state unchanged.
Timestamps must advance by less than 2^31 ms; maxGapMs is in that same horizon.
Output is a derived value only: applications should retain raw observations separately. No anti-aliasing guarantee.

## Minimal complete usage

```cpp
#include <SuperJupiterSignalFilter.h>
using namespace superjupiter::signal_filter;
int main() {
SignalFilter filter(Config(1000,5000));
filter.push(20,0);
Result result=filter.push(21,100);
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-signal-filter/blob/v0.1.0/src/SuperJupiterSignalFilter.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Stable convex smoothing preserves constant extreme values.
