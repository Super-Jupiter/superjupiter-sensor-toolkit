# SuperJupiter TimeAnchor API

Include `SuperJupiterTimeAnchor.h`. All public types/functions are inside `superjupiter::time_anchor`.

## Behaviour and limits

An anchor pairs a caller-supplied uint32 monotonic millisecond timestamp with nonnegative int64 UTC milliseconds.
Conversion accepts only forward distances below 2^31 ms and the configured maximum age.
Uncertainty is the anchor uncertainty plus driftPpm times elapsed time, rounded up.
Every setAnchor increments the revision and replaces the mapping; callers must record anchor changes.
No NTP, timezone conversion, clock synchronisation or guarantee of UTC correctness is provided.

## Minimal complete usage

```cpp
#include <SuperJupiterTimeAnchor.h>
using namespace superjupiter::time_anchor;
int main() {
TimeAnchor clock(Config(3600000,100));
clock.setAnchor(1000,1700000000000LL,20);
Result result=clock.convert(1500);
(void)result;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-time-anchor/blob/v0.1.0/src/SuperJupiterTimeAnchor.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
