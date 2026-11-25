# Legacy behaviors (deprecated)

The `crimild::behaviors` API is deprecated. Its behavior classes, context,
controller, tree, pointer alias, and free helper functions are marked with
`[[deprecated]]`, as is the associated `crimild::messaging::BehaviorEvent`.
Use `crimild::experimental::behaviors` for new behavior trees; its contract is
documented in [the experimental behavior README](../experimental/behaviors/README.md).

This is a deprecation notice, not a removal or an automatic migration. Existing
headers, signatures, implementations, and serialized RTTI names remain available.
Using the legacy API produces compiler deprecation warnings. Projects that treat
those warnings as errors may need to adjust their warning policy while migrating.

The experimental API has a different lifecycle and does not yet provide a
replacement for every legacy action, condition, or integration helper. Port those
callers deliberately rather than changing namespaces mechanically.
