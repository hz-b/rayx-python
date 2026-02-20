# tests/test_data.py
import sys
from pathlib import Path
import pytest
import pandas as pd

sys.path.insert(0, str(Path(__file__).parent.parent / "python"))

import rayx

RML_FILE = Path(__file__).parent / "res" / "test.rml"


@pytest.fixture(scope="module")
def rays():
    bl = rayx.import_beamline(str(RML_FILE))
    return bl.trace()


@pytest.fixture(scope="module")
def df(rays):
    return rayx.rays_to_df(rays)


def test_rays_to_df_returns_dataframe(df):
    assert isinstance(df, pd.DataFrame)

def test_rays_to_df_default_columns(df):
    expected_columns = [
        "path_event_id",
        "position_x", "position_y", "position_z",
        "direction_x", "direction_y", "direction_z",
        "electric_field_x", "electric_field_y", "electric_field_z",
        "energy", "order",
        "last_element_id", "source_id",
        "event_type",
    ]
    assert list(df.columns) == expected_columns

def test_rays_to_df_not_empty(df):
    assert len(df) > 0

def test_rays_to_df_custom_columns(rays):
    custom = ["energy", "path_event_id", "position_x"]
    df_custom = rayx.rays_to_df(rays, columns=custom)
    assert list(df_custom.columns) == custom

def test_rays_to_df_numeric_values(df):
    numeric_cols = ["energy", "position_x", "position_y", "position_z", "direction_x"]
    for col in numeric_cols:
        assert pd.api.types.is_numeric_dtype(df[col]), f"{col} should be numeric"
