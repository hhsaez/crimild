# Experimental behaviors

## Blackboards

`crimild::experimental::Blackboard` is a Resource (and therefore an Entity).
Its variant-backed entries can be accessed with typed keys or dynamic names.
Entity, Node, and Camera pointers are stored as `weak_ptr`, whether the input is
shared or weak. Pointer reads return `optional<shared_ptr<T>>`, locking the weak
reference; missing, mismatched, empty, or expired references return `nullopt`.
`contains()` checks the name only, so an expired entry can still be present.
Objects need an owner outside the board. A successful read keeps an object alive
only while the returned shared pointer is held; scalar and string values are copied.

```cpp
Key<std::shared_ptr<Entity>> Target { "ai.target" };
board.set(Target, enemy);                        // weak storage from a shared input
board.set(Target, std::weak_ptr<Entity>{enemy});  // same storage from a weak input
auto target = board.get(Target);                 // optional<shared_ptr<Entity>>
```

Pass `Blackboard&` to `tick()`, `initialize()`, and `update()`. The same
board reference reaches child ticks. Siblings exchange
information through one board, and entries persist across ticks. The caller
supplies the same board throughout an activation; Behavior does not store or
validate its identity. Base initialization sets RUNNING; overrides chain to their
immediate base first.

The caller keeps the board alive until the root tick returns, including across
callbacks that release other owners. When the caller holds a weak handle, lock it
once before entering the tree:

```cpp
if (auto board = weakBoard.lock()) {
   root.tick(*board);
}
```

Execution borrows the board without per-node reference-count updates or null
checks. Base Behavior has no Blackboard member. A behavior that needs a handle
beyond the call can explicitly use `retain(&blackboard)` to obtain a shared pointer,
then store it as shared or weak according to its lifetime needs. Retaining requires
an existing shared owner; a stack board can be borrowed by behaviors that do not
retain it.

Cleanup receives `terminate(state)` with SUCCESS, FAILURE, or ABORTED, matching
`getState()`. `abort()`, `attach()`, `detach()`, and `detachFromParent()` receive no
Blackboard. Structural changes still abort active work and propagate cancellation
to running children, including when automatically reparenting between trees that
use different boards. Cleanup relies on resources or handles captured during
execution. A subclass may retain a weak board reference for optional bookkeeping,
but must still clean up when that board has expired.

Systems Context migration, scoped lookup, and Blackboard entry serialization
remain separate work. Resource inheritance does not add serialization of entries.
Strong storage for global objects or a configurable ownership policy (possibly
templated) may be considered in a future iteration; the current board is weak.

Scene Event nodes now supply the root execution boundary: they hold the behavior
and Blackboard handles, while OnTick publishes a Clock value before execution.
See [Event behavior execution](../nodes/events/README.md) for ownership,
replacement, and TickSystem integration. Node disable/removal cancellation remains
a follow-up; this integration does not add those node lifecycle hooks.

## INVARIANTS

- `update()` must return `SUCCESS`, `FAILURE`, or `RUNNING`.
- `READY` and `ABORTED` are lifecycle states managed exclusively by the behavior
  infrastructure. Implementations must not return them from `update()`.
- A newly constructed behavior is `READY`.
- `tick()` starts a new activation when the behavior is not already `RUNNING`,
  calling `initialize()` once before the activation's first `update()`.
- `Behavior::initialize()` sets the state to `RUNNING`. Overrides must call their
  immediate base implementation first, before custom initialization or callbacks.
  After initialization, `tick()` asserts that the state is `RUNNING` or `ABORTED`.
- Returning `SUCCESS` or `FAILURE` completes the activation and calls
  `terminate(state)` once, with the final state already stored in `getState()`.
- `abort()` interrupts a running activation, stores `ABORTED`, and calls
  `terminate(ABORTED)` once. Calling it on a behavior that is not running has no effect.
- Composite and decorator termination must abort any children that remain running.
  Overrides of `terminate(state)` must chain to their base implementation with the
  same state for cleanup. Running children are interrupted through `abort()`, so
  they receive `terminate(ABORTED)` even when the parent completed normally.
- `terminate(state)` receives only `SUCCESS`, `FAILURE`, or `ABORTED`; the argument
  agrees with `getState()`. State is stored before cleanup, so an `abort()` called
  recursively during termination has no effect.
- Cleanup depends on resources or handles captured during execution. Termination,
  cancellation, attachment, and detachment do not require an execution context.
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
- Base initialization enters `RUNNING` before derived initialization work, so
  cancellation also works during the first tick. Cancellation inside a hook is
  preserved by `tick()` without calling termination twice. An `ABORTED` result
  from an earlier activation restarts on the next tick; an abort during the new
  initialization skips that tick's update.
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
