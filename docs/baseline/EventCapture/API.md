# SuperJupiter EventCapture API

Include `SuperJupiterEventCapture.h`. All public types/functions are inside `superjupiter::event_capture`.

## Behaviour and limits

Before a trigger, retain the latest Pre observations. trigger snapshots their chronological order.
The next Post observations complete the capture. Repeated triggers do not overwrite an active or completed event.
Raw double readings including NaN are preserved. Caller uint32 timestamps must advance by 1..2^31-1 ms.
Trigger has no timestamp of its own; its boundary is immediately after the last accepted observation.
Captured data is stable until reset; reset discards both history and event. No storage, interrupts or trigger logic.

## Minimal complete usage

```cpp
#include <SuperJupiterEventCapture.h>
using namespace superjupiter::event_capture;
int main() {
EventCapture<8,4> capture;
capture.push(20,0);capture.push(21,100);
capture.trigger();
capture.push(22,200);
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-event-capture/blob/v0.1.0/src/SuperJupiterEventCapture.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Overflow-safe capacity validation and chronological ring indexing.
