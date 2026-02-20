"""
RAY-X Python bindings
"""
import sys
from pathlib import Path
import pandas as pd

# Import the C++ extension module
try:
    from . import _core
except ImportError:
    # During development, try to find the built module
    import os
    build_dir = Path(__file__).parent.parent.parent / "build" / "install" / "rayx"
    if build_dir.exists():
        sys.path.insert(0, str(build_dir.parent))
        from rayx import _core
    else:
        raise ImportError("Cannot find compiled _core module. Did you build the project?")

def rays_to_df(rays, columns: list | None = None) -> pd.DataFrame:
    if columns is None:
        columns = [
            "path_event_id",
            "position_x", "position_y", "position_z",
            "direction_x", "direction_y", "direction_z",
            "electric_field_x", "electric_field_y", "electric_field_z",
            "energy", "order",
            "last_element_id", "source_id",
            "event_type",
        ]

    df = pd.DataFrame({col: getattr(rays, col) for col in columns})
    return df
