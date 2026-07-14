# tests/test_orientation.py
"""Tests for the glm::dmat4x4 <-> numpy caster behind the `orientation` property.

See the caster in main.cpp for the row/col convention and the rayx-core caveat:
elements only persist the 3x3 rotation block, so these tests assert the 3x3 block
for elements and the full 4x4 for sources.
"""
from pathlib import Path

import numpy as np
import pytest

import rayx

RML_FILE = Path(__file__).parent.parent / "examples" / "METRIX_U41_G1_H1_318eV_PS_MLearn_v114.rml"


@pytest.fixture(scope="module")
def beamline():
    return rayx.import_beamline(str(RML_FILE))


@pytest.fixture
def element(beamline):
    return beamline.elements[0]


@pytest.fixture
def source(beamline):
    return beamline.sources[0]


# --- read ---------------------------------------------------------------

def test_element_orientation_reads_as_4x4(element):
    a = np.asarray(element.orientation)
    assert a.shape == (4, 4)
    assert a.dtype == np.float64


def test_source_orientation_reads_as_4x4(source):
    a = np.asarray(source.orientation)
    assert a.shape == (4, 4)
    assert a.dtype == np.float64


def test_source_orientation_homogeneous_row(source):
    # DesignSource fills the homogeneous parts, so it is a clean rigid-transform matrix.
    a = np.asarray(source.orientation)
    assert np.allclose(a[3, :], [0, 0, 0, 1])
    assert np.allclose(a[:3, 3], [0, 0, 0])


# --- write + read-back --------------------------------------------------

def test_element_write_rotation_block_persists(element):
    # A proper rotation (90 deg about z) written into the 3x3 block must read back exactly.
    m = np.eye(4)
    m[:3, :3] = [[0, -1, 0], [1, 0, 0], [0, 0, 1]]
    element.orientation = m
    back = np.asarray(element.orientation)
    assert np.allclose(back[:3, :3], m[:3, :3])


def test_source_write_full_matrix_persists(source):
    m = np.eye(4)
    m[:3, :3] = [[0, 0, 1], [0, 1, 0], [-1, 0, 0]]
    source.orientation = m
    assert np.allclose(np.asarray(source.orientation), m)


def test_write_accepts_nested_list(source):
    m = [[1, 0, 0, 0], [0, 1, 0, 0], [0, 0, 1, 0], [0, 0, 0, 1]]
    source.orientation = m
    assert np.allclose(np.asarray(source.orientation), np.eye(4))


def test_column_convention_not_transposed(source):
    # An asymmetric (but homogeneous-bottom-row) matrix must come back un-transposed,
    # proving python[row][col] maps to the same logical element (not its transpose).
    m = np.array(
        [[2.0, 3.0, 5.0, 0.0],
         [7.0, 11.0, 13.0, 0.0],
         [17.0, 19.0, 23.0, 0.0],
         [0.0, 0.0, 0.0, 1.0]]
    )
    source.orientation = m
    assert np.allclose(np.asarray(source.orientation), m)


# --- bad input rejected -------------------------------------------------

@pytest.mark.parametrize("bad", [np.eye(3), np.arange(16.0), np.zeros((4, 3)), 5.0])
def test_wrong_shape_rejected(source, bad):
    with pytest.raises(TypeError):
        source.orientation = bad


# --- round-trip identity through tracing --------------------------------

def _stack(rays):
    return np.column_stack(
        [
            np.asarray(rays.path_id),
            np.asarray(rays.object_id),
            np.asarray(rays.position_x),
            np.asarray(rays.position_y),
            np.asarray(rays.position_z),
        ]
    )


def test_roundtrip_orientation_leaves_trace_unchanged(beamline):
    # Capture every component's orientation, trace, reassign np.asarray(orientation)
    # back verbatim, trace again with the same fixed seed, and assert the rays match.
    SEED = rayx.FIXED_SEED
    baseline = _stack(beamline.trace(seed=SEED))

    for comp in list(beamline.elements) + list(beamline.sources):
        comp.orientation = np.asarray(comp.orientation)

    after = _stack(beamline.trace(seed=SEED))
    assert np.array_equal(baseline, after)
