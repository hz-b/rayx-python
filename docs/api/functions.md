# Utility functions

These helpers are useful, but they are secondary to the main tracing workflow documented on the [Beamline and Rays](beamline.md) page.

## Small utility functions

These helpers exist, but they are secondary to the core tracing workflow:

- `rayx.get_info()`: return version and installation information
- `rayx.list_devices(...)`: list available tracing devices
- `rayx.fix_seed(...)` / `rayx.random_seed()`: control the global random seed used for tracing

:::{note}
These utility functions are exposed from the nanobind layer in `rayx/main.cpp`. They are Python-facing helpers rather than direct wrappers around one large C++ subsystem in `rayx-core`.
:::
