# Sources and elements

`beamline.sources` and `beamline.elements` expose C++ objects through the Python bindings. These objects have many reflected properties, but most users only need a small subset.

This page documents the most useful properties for inspection and tracing workflows. It is not an exhaustive list of every reflected field.

:::{note}
These Python objects come from reflected C++ data models. The binding-side reflection metadata is defined in `rayx/main.cpp`, while the underlying source and element structures live in `extern/rayx/Intern/rayx-core/src/Design/*` and `extern/rayx/Intern/rayx-core/src/Element/*`.
:::

## Sources

Source objects describe how rays are emitted into the beamline.

Commonly used properties:

- `name`: source name
- `type`: source type
- `energy`: photon energy
- `numberOfRays`: number of rays emitted by the source
- `position`: source position
- `orientation`: source orientation
- `photonFlux`: flux metadata when available

Representative example:

```python
for source in beamline.sources:
    print(
        source.name,
        source.type,
        source.energy,
        source.numberOfRays,
    )
```

### Source kinds

The currently exposed source kinds are:

- Point source (`POINT_SOURCE`)
- Matrix source (`MATRIX_SOURCE`)
- Dipole source (`DIPOLE_SOURCE`)
- Pixel source (`PIXEL_SOURCE`)
- Circle source (`CIRCLE_SOURCE`)
- Simple undulator source (`SIMPLE_UNDULATOR_SOURCE`)
- Ray list source (`RAY_LIST_SOURCE`)

Other source properties exist for specific source models, such as divergence, opening angles, energy spread, and undulator parameters.

## Elements

Element objects describe the optical system through which rays propagate.

Commonly used properties:

- `name`: element name
- `type`: element type
- `position`: element position
- `orientation`: element orientation
- `material`: substrate material
- `grazingIncAngle`: grazing incidence angle for mirror- and grating-like elements
- `lineDensity`: line density for gratings
- `radius`, `longRadius`, `shortRadius`: representative curvature parameters
- `orderOfDiffraction`: diffraction order for grating elements

Representative example:

```python
for element in beamline.elements:
    print(element.name, element.type)
```

The element kind names below come from the exposed `ElementType` enum in the bindings, and `element.type` returns one of those values.

### Element kinds

- Cone mirror (`CONE_MIRROR`)
- Crystal (`CRYSTAL`)
- Cylindrical mirror (`CYLINDRICAL_MIRROR`)
- Ellipsoid mirror (`ELLIPSOID_MIRROR`)
- Experts mirror (`EXPERTS_MIRROR`)
- Foil (`FOIL`)
- Image plane (`IMAGE_PLANE`)
- Paraboloid mirror (`PARABOLOID_MIRROR`)
- Plane grating (`PLANE_GRATING`)
- Plane mirror (`PLANE_MIRROR`)
- Reflection zoneplate (`REFLECTION_ZONEPLATE`)
- Slit (`SLIT`)
- Sphere (`SPHERE`)
- Sphere grating (`SPHERE_GRATING`)
- Sphere mirror (`SPHERE_MIRROR`)
- Toroid grating (`TOROID_GRATING`)
- Toroid mirror (`TOROID_MIRROR`)

#### Mirrors

Mirror elements cover the reflective optics in the beamline, including plane, spherical, cylindrical, ellipsoidal, paraboloidal, toroidal, cone, and expert-defined variants.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `material`: substrate material
- `surfaceCoatingType`, `materialCoating`, `thicknessCoating`, `roughnessCoating`: coating configuration when present
- `grazingIncAngle`: grazing incidence angle for mirror geometries that use it
- `radius`, `shortRadius`, `longRadius`: curvature parameters for spherical, cylindrical, and toroidal shapes
- `entranceArmLength`, `exitArmLength`: arm lengths used by several curved mirror models
- `longHalfAxisA`, `shortHalfAxisB`: ellipsoid parameters
- `armLength`, `parameterP`, `parameterPType`: paraboloid parameters
- `expertsOptics`, `expertsCubic`: reflected expert-mode geometry definitions

Representative kinds:

- `PLANE_MIRROR`
- `SPHERE_MIRROR`
- `CYLINDRICAL_MIRROR`
- `ELLIPSOID_MIRROR`
- `PARABOLOID_MIRROR`
- `TOROID_MIRROR`
- `CONE_MIRROR`
- `EXPERTS_MIRROR`
- `SPHERE`

:::{note}
Mirror parameters are imported through functions such as `getPlaneMirror`, `getSphereMirror`, `getCylinder`, `getEllipsoid`, `getParaboloid`, `getToroidMirror`, `getCone`, `getExpertsOptics`, and `getExpertsCubic` in `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`.
:::

#### Gratings

Grating elements add diffraction-specific parameters on top of their geometry.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `lineDensity`: groove density
- `orderOfDiffraction`: active diffraction order
- `radius`, `shortRadius`, `longRadius`: curvature parameters for spherical and toroidal gratings
- `deviationAngle`, `entranceArmLength`, `exitArmLength`: geometry parameters used by spherical gratings
- `surfaceCoatingType`, `materialCoating`, `thicknessCoating`, `roughnessCoating`: coating metadata when present

Representative kinds:

- `PLANE_GRATING`
- `SPHERE_GRATING`
- `TOROID_GRATING`

:::{note}
The common grating setup is assembled in `getGrating`, with shape-specific import in `getPlaneGrating`, `getSphereGrating`, and `getToroidalGrating` inside `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`. Runtime grating behavior is created by `makeGrating` in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.cpp`.
:::

#### Slit

`Slit` elements model an opening, optionally with a central beamstop, in the `XY` design plane.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `cutout`: overall outer shape imported from the beamline description
- `openingShape`: opening geometry, such as rectangular or elliptical
- `openingWidth`, `openingHeight`: size of the transmitting opening
- `centralBeamstop`: whether a central stop is present
- `stopWidth`, `stopHeight`: size of the beamstop when used
- `totalWidth`, `totalHeight`: full slit body dimensions
- `distancePreceding`: spacing metadata used in imported beamlines

:::{note}
In the C++ core, slit import and parameter loading live in `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h` (`getSlit`), and the tracing behavior is built in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.cpp` (`makeSlit`).
:::

#### Image plane

`Image plane` elements are detector-like planes used to collect traced rays.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `cutout`: active area geometry
- `material`: substrate or material metadata when present

Unlike reflective or diffractive optics, image planes do not add many element-specific reflected parameters in the current Python surface.

:::{note}
The C++ import path for image planes is `getImageplane` in `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`. Their tracing behavior maps to the `ImagePlane` branch in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.h`.
:::

#### Foil

`Foil` elements represent thin transmissive interaction elements.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `cutout`: active area geometry
- `material`: substrate material metadata
- `thicknessSubstrate`: foil or substrate thickness
- `roughnessSubstrate`: substrate roughness

:::{note}
Foil import is handled by `getFoil` in `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`, and the traced foil behavior is assembled by `makeFoil` in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.cpp`.
:::

#### Crystal

`Crystal` elements expose the parameters needed for crystal diffraction behavior.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `cutout`: active area geometry
- `material`: general material metadata
- `crystalType`: crystal model or configuration kind
- `offsetAngle`: crystal offset angle
- `structureFactorReF0`, `structureFactorImF0`: forward structure-factor components
- `structureFactorReFH`, `structureFactorImFH`: diffracted structure-factor components
- `structureFactorReFHC`, `structureFactorImFHC`: conjugate structure-factor components
- `unitCellVolume`: crystal unit cell volume
- `dSpacing2`: lattice-spacing parameter used by the crystal model

:::{note}
The import-side crystal setup is in `getCrystal` inside `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`, and the runtime crystal behavior is built by `makeCrystal` in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.cpp`.
:::

#### Reflection zoneplate

`Reflection zoneplate` elements expose design parameters for reflection zoneplate tracing.

Commonly used properties:

- `name`, `type`, `position`, `orientation`: identity and placement
- `cutout`: active area geometry
- `fresnelZOffset`: Fresnel zone offset
- `designSagittalEntranceArmLength`, `designSagittalExitArmLength`: sagittal design arm lengths
- `designMeridionalEntranceArmLength`, `designMeridionalExitArmLength`: meridional design arm lengths
- `designEnergy`: design energy used to derive the tracing wavelength
- `designOrderOfDiffraction`, `orderOfDiffraction`: design and active diffraction orders
- `designAlphaAngle`, `designBetaAngle`: design incidence and exit angles
- `imageType`: zoneplate imaging mode flag
- `additionalOrder`: additional diffraction-order handling
- `shortRadius`, `longRadius`: curvature parameters for non-planar zoneplate variants

:::{note}
Reflection zoneplate import lives in `getRZP` in `extern/rayx/Intern/rayx-core/src/Rml/DesignElementWriter.h`, and the corresponding behavior object is created by `makeRZPBehaviour` in `extern/rayx/Intern/rayx-core/src/Element/Behaviour.cpp`.
:::

```python
grating = beamline["Spherical Grating"]
print(grating.lineDensity)
print(grating.orderOfDiffraction)
print(grating.radius)
```

Many additional reflected properties are available for specialized optics, coatings, cutouts, crystals, and expert-mode configurations. Advanced users should consult the binding definitions in `rayx/main.cpp` and the underlying C++ model if they need the full surface.
