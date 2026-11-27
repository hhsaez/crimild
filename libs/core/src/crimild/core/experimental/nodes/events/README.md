# Event behavior execution

`Event` is a scene node with a behavior handle and a Blackboard handle. Each event
executes its own behavior instance; different events may share a Blackboard.
Events create an empty board by default. Setting no behavior makes triggering a
no-op; a configured behavior without a board logs an error and returns FAILURE.

`trigger()` returns the behavior's state. A RUNNING activation resumes on the next
trigger; triggering after completion starts another activation. Disabled events
are skipped, without changing the behavior's state.

Replacing the behavior or the board aborts active work before changing the member.
Assigning the same handle is a no-op. Cleanup callbacks may choose a replacement
configuration or restart the current activation; the outer setter preserves that
choice. Triggering
keeps local shared handles alive while the behavior borrows `Blackboard&`, even
when a callback replaces the event's members.

`OnTick::trigger(const Clock&)` stores a Clock value under
`crimild::experimental::keys::Clock` (`"crimild.clock"`) before executing the
behavior. Blackboard stores and returns Clock copies using Clock's existing copy
semantics; pointer entries retain their existing weak storage policy. Existing
`OnTick::triggered` signal subscribers are notified after behavior execution.

`crimild::experimental::systems::TickSystem` obtains the scene from Systems
Context and traverses enabled nodes through the Tick visitor. One Clock is kept
per system and supplied to every event in a frame. Behavior SUCCESS/FAILURE does
not determine System success/failure; missing scenes and child System results
retain their normal System meanings. The universal scene runner uses TickSystem;
its former SimulationSystem name remains as a deprecated compatibility class.

## Follow-ups

- Wire node disable, detachment, and scene removal to abort affected behaviors.
  Skipping a disabled node currently pauses its activation; it does not cancel it.
- Add other event kinds and their execution contracts, including synchronous
  OnCollision handlers. This change adds no collision system or event.
- Measure traversal before introducing event registries or caching.
- Tree serialization, Blackboard entry serialization, and Systems Context
  migration are separate changes.
