"""Type stub for the public ``rayx`` API.

The native bindings are re-exported from the private ``_core`` extension, which is
typed by the auto-generated ``_core.pyi``. Only the small Python helper layer is
declared here by hand. Keep this in sync with ``__init__.py`` — ``tests/test_stubs.py``
enforces it with ``stubtest``.
"""
from . import _core as _core
from ._core import *
from ._version import *
from .data import rays_to_df as rays_to_df

def get_info() -> dict[str, object]: ...
