# Tutorial: tracing a beamline from an RML file

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
| `device_type` | `DeviceType.Cpu` | Compute device; CPU is the only supported option in this Python package |
| `max_events` | `None` | Maximum number of scattering events per ray |
| `seed` | `None` | RNG seed; `None` draws a random seed |

For reproducible results, fix the seed before tracing:

```python
rayx.fix_seed()          # uses the built-in canonical seed
rays = beamline.trace()
```

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

Work with the DataFrame using standard pandas operations. For example, select only rays that were absorbed (i.e. reached the image plane or a detector element):

```python
from rayx import EventType

absorbed = df[df["event_type"] == EventType.ABSORBED]
print(f"{len(absorbed)} rays absorbed out of {len(df)}")
```

Or filter by a specific optical element (using its index in the element list):

```python
# Rays that hit the second element (index 1)
on_m1 = df[df["object_id"] == 1]
print(on_m1[["position_x", "position_y", "position_z"]].describe())
```

---

:::{seealso}
For a higher-level interface that integrates rayx-python into a full simulation and optimisation workflow — including energy-angle sweeps, batch runs, and parameter studies — see [raypyng](https://raypyng.readthedocs.io/en/latest/).
:::
