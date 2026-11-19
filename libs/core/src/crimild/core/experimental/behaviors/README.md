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
- A subsequent `tick()` starts a new activation after completion or cancellation.

Invalid `update()` results violate the contract and trigger `CRIMILD_INVARIANT`
in debug builds inside `tick()`. Release builds evaluate neither invariant
argument. Cancellation is exercised through `abort()`, not an `ABORTED` return.

## Ownership and structural edits

- Each behavior instance has at most one structural parent, shared by composites
  and decorators. The parent owns its children; the child holds a non-owning pointer.
- Null composite attachments and repeated attachments to the same parent are no-ops.
  A null decorator attachment detaches its current child.
- Self-attachment and ancestor cycles are logged with `CRIMILD_LOG_ERROR` and
  rejected without changing the hierarchy.
- Attachment automatically detaches a child from its previous owner, matching
  experimental nodes. `detachFromParent()` also supports explicit detachment and
  retains the child while its owner releases it. Both composites and decorators
  participate in reparenting; a behavior still has at most one parent.
- Rejected operations and no-ops do not interrupt execution. Before a real child
  collection change, the composite or decorator aborts its active activation.
  Detachment also aborts the detached child if it is running.
- Parent destruction clears the parent pointers of retained children.
- An activation is running before `initialize()` and `update()` are called, so
  cancellation also works during its first tick. Cancellation inside a hook is
  preserved by `tick()` without calling termination twice.
- Traversal stops immediately after a child tick cancels the activation, before
  accessing activation data that termination may have cleared. A cancelled child
  cancels its selecting composite through `abort()`.
- Structural edits replace the earlier deferred-attachment policy: the next tick
  restarts traversal with a fresh snapshot, rather than resuming the edited one.

## ActiveSequence

`composites::ActiveSequence` starts traversal at the first child on every tick.
It continues only after `SUCCESS`, returns the first `FAILURE` or `RUNNING`, and
succeeds when all children succeed. An empty active sequence succeeds. When an
earlier child blocks traversal, later running children are aborted before returning.
Child cancellation cancels the active sequence and stops traversal immediately.

Conditions remain reusable instant checks: `SUCCESS` means true and `FAILURE`
means false. For example, attach `EnemyInRange` followed by a remembered `Sequence`
of aim and fire-burst actions. Each tick rechecks range while the inner sequence
resumes its current action. If range becomes false, the running action is aborted
before another update. Rechecking does not reinitialize a still-running child.

Put multiple actions inside a remembered `Sequence` when their progress should be
preserved. Successful children attached directly to `ActiveSequence` are ticked
again on subsequent ticks and can repeat their side effects. A `RUNNING` condition
blocks later actions; it does not authorize them to run alongside the condition.

Structural edits follow the normal composite ownership and cancellation rules;
the next tick starts traversal again with the current children.
