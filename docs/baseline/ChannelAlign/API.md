# SuperJupiter ChannelAlign API

Include `SuperJupiterChannelAlign.h`. All public types/functions are inside `superjupiter::channel_align`.

## Behaviour and limits

Retains two finite samples per channel; channels must use the same uint32 millisecond clock.
Each channel advances by 1..2^31-1 ms. Query must remain within that channel's half-range clock horizon.
Exact samples succeed. Between retained samples interpolation is allowed only below maxInterpolationMs.
After the newest sample, optional hold-last succeeds up to maxHoldMs and reports Held. No extrapolation before history.
Different physical units are not combined. No implicit alignment or claim that interpolation reconstructs true readings.

## Minimal complete usage

```cpp
#include <SuperJupiterChannelAlign.h>
using namespace superjupiter::channel_align;
int main() {
ChannelAlign<2> channels(1000,100);
channels.push(0,20,0);channels.push(0,22,1000);
Result aligned=channels.at(0,500);
(void)aligned;
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-channel-align/blob/v0.1.0/src/SuperJupiterChannelAlign.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Invalid time limits consistently rejected; interpolation uses stable convex blending.
