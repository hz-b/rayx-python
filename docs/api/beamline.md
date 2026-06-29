# `Beamline` and `Rays`

This page covers the main tracing workflow: loading a beamline, inspecting it, running a trace, and working with the resulting event data.

## `rayx.import_beamline(path)`

Load a beamline definition from an `.rml` file and return a `Beamline` object.

### Parameters

- `path`: path to a beamline file in RML format

### Returns

- `Beamline`

### Typical use

```python
import rayx

beamline = rayx.import_beamline("path/to/beamline.rml")
```

### Common failure modes

- the file does not exist
- the file is not a valid RML beamline
- referenced resources cannot be resolved

:::{note}
On the C++ side, `rayx.import_beamline(...)` is exposed from `rayx/main.cpp` and delegates to the RML import/parsing layer in `extern/rayx/Intern/rayx-core/src/Rml/*`.
:::

`Beamline` is the main object you work with after loading an `.rml` file. It contains the sources and optical elements that define a tracing setup.

:::{note}
The Python `Beamline` object is backed by the beamline/domain model in `extern/rayx/Intern/rayx-core/src/Beamline/*`. Name lookup and tracing entrypoints are exposed through the binding layer in `rayx/main.cpp`.
:::

## `beamline.sources`

Returns the beamline sources as a sequence of source objects.

Typical use:

```python
for source in beamline.sources:
    print(source.name, source.type, source.energy)
```

## `beamline.elements`

Returns the optical elements as a sequence of element objects.

Typical use:

```python
for element in beamline.elements:
    print(element.name, element.type)
```

## `beamline["name"]`

Look up a source or element by its `name` property.

```python
grating = beamline["Spherical Grating"]
print(grating.lineDensity)
```

If no source or element with that name exists, the lookup raises a runtime error.

## `beamline.trace(sequential=False, seed=None, max_events=None, device_index=None, device_type=rayx.DeviceType.All)`

Run a ray trace and return a `Rays` object.

### Parameters

- `sequential`: if `True`, rays interact with elements in beamline order; otherwise tracing is non-sequential
- `seed`: optional integer seed for deterministic traces
- `max_events`: optional cap on the number of events recorded per ray in non-sequential mode
- `device_index`: optional device index from `rayx.list_devices()`
- `device_type`: restrict tracing to a class of device such as CPU or GPU

### Returns

- `Rays`

### Typical use

```python
rays = beamline.trace()
```

```python
rays = beamline.trace(
    sequential=False,
    seed=1234,
    max_events=20,
    device_type=rayx.DeviceType.Cpu,
)
```

The returned `Rays` object stores event data column-wise and can be inspected directly or converted with `rayx.rays_to_df(...)`.

:::{note}
`Beamline.trace(...)` is exposed from `rayx/main.cpp`. The tracing pipeline itself lives in `extern/rayx/Intern/rayx-core/src/Tracer/*`, where device selection, ray generation, propagation, and event recording are implemented.
:::

## `Rays`

`Beamline.trace(...)` returns a `Rays` object. It stores the recorded event data column-wise, with one NumPy array per attribute.

:::{note}
`Rays` is backed by the C++ result container in `extern/rayx/Intern/rayx-core/src/Rays.h` and related tracing code. The bindings expose its stored vectors to Python as NumPy array views.
:::

### What `Rays` contains

The object exposes one-dimensional arrays such as:

- `path_id`
- `path_event_id`
- `position_x`, `position_y`, `position_z`
- `direction_x`, `direction_y`, `direction_z`
- `electric_field_x`, `electric_field_y`, `electric_field_z`
- `optical_path_length`
- `energy`
- `order`
- `object_id`
- `source_id`
- `event_type`

Typical use:

```python
rays = beamline.trace()

print(rays.position_x.shape)
print(rays.energy[:10])
```

### When to use direct arrays

Use direct `Rays` properties when you want:

- NumPy-native workflows
- low-overhead access to event columns
- custom analysis without creating a DataFrame

## `rayx.rays_to_df(rays, columns=None)`

Convert a `Rays` object into a {class}`pandas.DataFrame`.

This is the easiest way to inspect trace output when you want tabular analysis, filtering, or plotting with the pandas ecosystem.

### Parameters

- `rays`: a `Rays` instance returned by `Beamline.trace(...)`
- `columns`: optional list of attribute names to include; if omitted, a default set of event columns is used

### Default columns

- `path_id`
- `path_event_id`
- `position_x`, `position_y`, `position_z`
- `direction_x`, `direction_y`, `direction_z`
- `electric_field_x`, `electric_field_y`, `electric_field_z`
- `optical_path_length`
- `energy`
- `order`
- `object_id`
- `source_id`
- `event_type`

### Returns

- `pandas.DataFrame`

### Typical use

```python
df = rayx.rays_to_df(rays)
absorbed = df[df["event_type"] == rayx.EventType.ABSORBED.value]
```

Use `rayx.rays_to_df(...)` when you want:

- easy filtering by event type or object id
- summary statistics with pandas
- integration with plotting or reporting tools built around DataFrames

### Notes on event data

- each row-like event is represented across multiple arrays
- `event_type` is stored numerically, so comparisons are usually done against enum `.value`
- image-plane hits are typically identified by combining `object_id` and `event_type`

See also:

- [Utility functions](functions.md)
- [Sources and elements](sources-and-elements.md)
