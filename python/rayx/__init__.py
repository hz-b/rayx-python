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

# Add Python enhancements
def hello():
    """Test function to verify Python wrapper is included"""
    return "Hello from Python wrapper!"

def get_info():
    """Get information about the RAYX installation"""
    info = {
        "version": __version__,
        "python_wrapper": True,
        "cpp_module": str(_core.__file__),
        "module_path": str(Path(__file__).parent),
    }
    return info

def enhanced_function():
    """Pure Python function using C++ backend"""
    # This is a placeholder - customize based on your actual C++ functions
    # result = _core.cpp_function()  # Call C++ function
    # Add Python processing
    return "Enhanced function - add your logic here"

__version__ = "0.4.1"
__all__ = ['hello', 'get_info', 'enhanced_function']
