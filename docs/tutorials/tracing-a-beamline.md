# Tracing a beamline from an RML file

This tutorial walks through the core workflow: loading a beamline, running a ray trace, and analysing the results.

## 1. Import and verify

```python
import rayx

info = rayx.get_info()
print(info)
# {'version': '...', 'python_wrapper': '...', 'cpp_module': '...', 'module_path': '...'}
```

## 2. Load a beamline

Beamlines are defined in `.rml` files (RAY-X Markup Language). Pass the file path to {func}`rayx.import_beamline`:

```python
beamline = rayx.import_beamline("path/to/your/beamline.rml")
```

## 3. Inspect sources and elements

```python
# List all sources
for source in beamline.sources:
    print(source.name, source.type)

# List all optical elements
for element in beamline.elements:
    print(element.name, element.type)

# Look up by name
mirror = beamline["M1"]
print(mirror.grazingIncAngle)
```

## 4. Run a trace

Call {meth}`Beamline.trace` to perform the ray trace. It returns a {class}`rayx.Rays` object.

```python
rays = beamline.trace()
```

Key keyword arguments:

| Argument | Default | Description |
| --- | --- | --- |
| `sequential` | `False` | If `True`, rays interact with elements in the order they appear in the file |
| `max_events` | `None` | Maximum number of scattering events per ray |

## 5. Convert rays to a DataFrame

{func}`rayx.rays_to_df` converts the result to a {class}`pandas.DataFrame` with one row per ray event:

```python
df = rayx.rays_to_df(rays)
print(df.columns.tolist())
# ['path_id', 'path_event_id', 'position_x', 'position_y', 'position_z',
#  'direction_x', 'direction_y', 'direction_z', 'electric_field_x', ...
#  'energy', 'order', 'object_id', 'source_id', 'event_type']
print(df.shape)
```

## 6. Filter and analyse

The `event_type` column is stored as an unsigned integer — compare against the enum's `.value` attribute:

```python
from rayx import EventType

# Rays blocked by apertures/slits along the beamline
absorbed = df[df["event_type"] == EventType.ABSORBED.value]
print(f"{len(absorbed)} rays blocked by apertures")
```

Rays that reached the image plane (the last element in the list) appear with `event_type == EventType.HIT_ELEMENT`:

```python
image_plane_id = len(beamline.elements) - 1
on_image = df[
    (df["object_id"] == image_plane_id)
    & (df["event_type"] == EventType.HIT_ELEMENT.value)
]
print(f"{len(on_image)} rays on the image plane")
print(on_image[["position_x", "position_y", "position_z"]].describe())
```

## Complete example

```python
import rayx
from rayx import EventType

# Load the beamline
beamline = rayx.import_beamline("path/to/your/beamline.rml")

# Inspect what's in it
for source in beamline.sources:
    print(source.name, source.type, source.energy, "eV")
for i, element in enumerate(beamline.elements):
    print(i, element.name, element.type)

# Run the trace
rays = beamline.trace()

# Convert to a DataFrame
df = rayx.rays_to_df(rays)
print(df.shape)

# Event type breakdown (compare using .value — event_type column is uint32)
for et in EventType:
    count = (df["event_type"] == et.value).sum()
    if count > 0:
        print(f"{et.name}: {count}")

# Rays blocked by apertures/slits
absorbed = df[df["event_type"] == EventType.ABSORBED.value]
print(f"{len(absorbed)} rays blocked by apertures")

# Rays that reached the image plane (last element)
image_plane_id = len(beamline.elements) - 1
on_image = df[
    (df["object_id"] == image_plane_id)
    & (df["event_type"] == EventType.HIT_ELEMENT.value)
]
print(f"{len(on_image)} rays on the image plane")
print(on_image[["position_x", "position_y", "position_z"]].describe())
```

---

:::{seealso}
For a higher-level interface that integrates rayx-python into a full simulation and optimisation workflow — including energy-angle sweeps, batch runs, and parameter studies — see [raypyng](https://raypyng.readthedocs.io/en/latest/).
:::
