"""
RAY-X Python bindings
"""
import sys
from pathlib import Path

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

# Re-export everything from C++ module
from ._core import *

def get_info():
    """Get information about the RAYX installation"""
    info = {
        "version": __version__,
        "python_wrapper": True,
        "cpp_module": str(_core.__file__),
        "module_path": str(Path(__file__).parent),
    }
    return info

# From other files
from .data import rays_to_df

__version__ = "0.4.3"
__all__ = ['get_info', 'rays_to_df']
