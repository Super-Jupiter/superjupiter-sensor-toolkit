# SuperJupiter TimeAggregate API

Include `SuperJupiterTimeAggregate.h`. All public types/functions are inside `superjupiter::time_aggregate`.

## Behaviour and limits

Buckets start at the first accepted timestamp and are half-open [start,start+period).
push returns Completed when an observation enters a later bucket; takeCompleted reads the last completed bucket.
Skipped empty buckets are counted, not fabricated as measurements. Consume Completed results before another push.
Coverage is first-to-last finite observation span divided by period, not continuous sensing proof.
Invalid observations are counted and retained in bucket totals; means use finite samples only. No time weighting.

## Minimal complete usage

```cpp
#include <SuperJupiterTimeAggregate.h>
using namespace superjupiter::time_aggregate;
int main() {
TimeAggregate minutes(60000);
minutes.push(20,0);minutes.push(21,1000);minutes.push(22,60000);
Summary previous;
minutes.takeCompleted(previous);
return 0;
}

```

## Public declarations

The [public header](https://github.com/Super-Jupiter/superjupiter-time-aggregate/blob/v0.1.0/src/SuperJupiterTimeAggregate.h) is the complete API; constructors, result fields and status enums are visible there.
All time arguments are caller supplied. No method is thread-safe without caller synchronisation.
Non-success result fields must only be used where this contract explicitly defines them.
Memory is bounded by template capacities and local arrays; storage/network adapter implementations may have their own costs.


## 0.1.1 review clarifications

See [INTEROPERABILITY.md](INTEROPERABILITY.md) for units, timestamp horizons, numeric validity, payload constraints and storage synchronisation.

- Removed unused duplicated math helpers and unnecessary includes.
- Valid long consecutive intervals cross buckets and wraps correctly; stable means and explicit numeric validity retain accurate finite counts.

Summary.numericError invalidates the mean but not finite/invalid observation counts or min/max. Long valid consecutive intervals can cross more than the half-range relative to the current bucket origin.
