"""
RAY-X Python bindings
"""
import sys
from pathlib import Path

# Import the C++ extension module

# In development, we want to import from the local build directory

try:
    from . import core
    from .core import *
except ImportError:
    # If the import fails, we might be in a development environment where the module is not built yet
    # We can try to import from the build directory
    build_dir = Path(__file__).parent.parent / "build" / "rayx"
    if build_dir.exists():
        sys.path.insert(0, str(build_dir))
        try:
            import core
            from core import *
        except ImportError:
            raise ImportError("Could not import the RAYX C++ extension module. Make sure it is built and available.")
    else:
        raise ImportError("Could not import the RAYX C++ extension module. Make sure it is built and available.")




def get_info():
    """Get information about the RAYX installation"""
    info = {
        "version": __version__,
        "python_wrapper": True,
        "cpp_module": str(core.__file__),
        "module_path": str(Path(__file__).parent),
    }
    return info

# From other files
from .data import rays_to_df

__name__ = "rayx"
__version__ = "0.4.3"
__all__ = ['get_info', 'rays_to_df']