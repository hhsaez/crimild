# Experimental behaviors

## INVARIANTS

- `update()` must return `SUCCESS`, `FAILURE`, or `RUNNING`.
- `READY` and `ABORTED` are lifecycle states managed exclusively by the behavior
  infrastructure. Implementations must not return them from `update()`.
- A newly constructed behavior is `READY`.
- `tick()` starts a new activation when the behavior is not already `RUNNING`,
  calling `initialize()` once before the activation's first `update()`.
- Returning `SUCCESS` or `FAILURE` completes the activation and calls `terminate()`
  once, with the final state already stored.
- `abort()` interrupts a running activation, stores `ABORTED`, and calls
  `terminate()` once. Calling it on a behavior that is not running has no effect.
- Composite and decorator termination must abort any children that remain running.
  Overrides of `terminate()` must chain to their base implementation for cleanup.
- An activation is running before `initialize()` and `update()` are called, so
  cancellation works during the first tick. `tick()` preserves cancellation inside
  a hook without calling termination twice.
- Composite traversal stops immediately after a child cancels the activation,
  before accessing activation data that termination may have cleared. A cancelled
  child cancels its selecting composite through `abort()`.
- A subsequent `tick()` starts a new activation after completion or cancellation.

Invalid `update()` results violate the contract and trigger `CRIMILD_INVARIANT`
in debug builds inside `tick()`. Release builds evaluate neither invariant
argument. Cancellation is exercised through `abort()`, not an `ABORTED` return.
