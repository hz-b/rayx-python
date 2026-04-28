"""
RAY-X Python bindings
"""
import sys
from pathlib import Path

# Import the C++ extension module
from . import core

# Re-export everything from C++ module
from .core import *

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

__version__ = "0.4.3"
__all__ = ['get_info', 'rays_to_df']
