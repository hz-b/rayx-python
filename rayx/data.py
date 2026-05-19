"""
RAY-X Python bindings
"""
import pandas as pd
from . import core

def rays_to_df(rays: core.Rays, columns: list | None = None) -> pd.DataFrame:
    if columns is None:
        columns = [
            "path_id",
            "path_event_id",
            "position_x", "position_y", "position_z",
            "direction_x", "direction_y", "direction_z",
            "electric_field_x", "electric_field_y", "electric_field_z",
            "optical_path_length",
            "energy", "order",
            "object_id", "source_id",
            "event_type",
        ]

    df = pd.DataFrame({col: getattr(rays, col) for col in columns})
    return df
