"""
Example accompanying the "Tracing a beamline from an RML file" tutorial.
Uses the bundled test beamline (tests/res/test.rml).
"""

from pathlib import Path

import rayx
from rayx import EventType

RML_FILE = Path(__file__).parent.parent / "tests" / "res" / "test.rml"

# --- 1. Import and verify ---
info = rayx.get_info()
print("rayx version:", info["version"])

# --- 2. Load the beamline ---
beamline = rayx.import_beamline(str(RML_FILE))

# --- 3. Inspect sources and elements ---
print("\nSources:")
for source in beamline.sources:
    print(f"  {source.name!r}  type={source.type}  energy={source.energy} eV  rays={source.numberOfRays}")

print("\nElements:")
for i, element in enumerate(beamline.elements):
    print(f"  [{i}] {element.name!r}  type={element.type}")

# Look up a specific element by name
grating = beamline["Spherical Grating"]
print(f"\nSpherical Grating:")
print(f"  line density : {grating.lineDensity:.3f} lines/mm")
print(f"  order        : {grating.orderOfDiffraction}")
print(f"  radius       : {grating.radius:.1f} mm")

# --- 4. Run a trace ---
rays = beamline.trace()
print(f"\nTrace complete.")

# --- 5. Convert to DataFrame ---
df = rayx.rays_to_df(rays)
print(f"DataFrame shape: {df.shape}")

# --- 6. Filter and analyse ---
# event_type is stored as uint32 — compare against the enum's integer value
print("\nEvent type breakdown:")
for et in EventType:
    count = (df["event_type"] == et.value).sum()
    if count > 0:
        print(f"  {et.name}: {count}")

# Rays absorbed by apertures/slits along the beamline
absorbed = df[df["event_type"] == EventType.ABSORBED.value]
print(f"\nRays blocked by apertures: {len(absorbed)}")

# Rays that reached the image plane (last element)
image_plane_id = len(beamline.elements) - 1
on_image = df[
    (df["object_id"] == image_plane_id)
    & (df["event_type"] == EventType.HIT_ELEMENT.value)
]
print(f"Rays on ImagePlane (object_id={image_plane_id}): {len(on_image)}")

if len(on_image) > 0:
    print("\nImagePlane spot statistics (mm):")
    print(on_image[["position_x", "position_y", "position_z"]].describe().to_string())
