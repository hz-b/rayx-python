"""
RAY-X Python bindings
"""
# The compiled extension `_core` is an implementation detail; the public API of
# this package is the native bindings re-exported here plus the Python helpers below.
from . import _core
from ._core import *

from ._version import *

# Python helper layer built on top of the native bindings.
from .data import rays_to_df


def get_info():
    "Get information about the RAYX installation"
    # Imported locally so it does not leak into the package's public namespace.
    from pathlib import Path

    info = {
        "version": __version__,
        "python_wrapper": True,
        "cpp_module": str(_core.__file__),
        "module_path": str(Path(__file__).parent),
    }
    return info


# Advertise the full native API (everything `_core` exports) plus the Python
# helpers and version, so `from rayx import *` mirrors the real public surface.
__all__ = [name for name in dir(_core) if not name.startswith("_")] + [
    "get_info",
    "rays_to_df",
    "__version__",
]
