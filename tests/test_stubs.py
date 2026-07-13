"""Guardrail: the hand-written public stub must stay in sync with the code.

`src/rayx/__init__.pyi` shadows `__init__.py` for type checkers, so any public Python
helper missing from it silently disappears from the typed API (no IDE completion, mypy
users can't reference it). The native `_core` names and version symbols are covered by
the stub's `from ._core import *` / `from ._version import *`; only the package's own
helpers need explicit declarations, which is what this test enforces.

The native API is typed separately by the auto-generated `_core.pyi` (no upkeep).
"""
import ast
import shutil
import subprocess
import types
from pathlib import Path

import rayx

STUB = Path(rayx.__file__).with_name("__init__.pyi")
# ABI3 extensions are named ``_core.abi3.so`` but their companion stub is
# always ``_core.pyi``.
CORE_STUB = Path(rayx._core.__file__).with_name("_core.pyi")


def _package_defined_helpers():
    """Public helpers the `rayx` package itself defines (mirrors test_init.py)."""
    helpers = set()
    for name, value in vars(rayx).items():
        if name.startswith("_") or isinstance(value, types.ModuleType):
            continue
        module = getattr(value, "__module__", "") or ""
        if (module == "rayx" or module.startswith("rayx.")) and not module.startswith("rayx._core"):
            helpers.add(name)
    return helpers


def _stub_declared_names():
    """Names explicitly declared or re-exported (non-star) in __init__.pyi."""
    tree = ast.parse(STUB.read_text())
    names = set()
    for node in tree.body:
        if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef)):
            names.add(node.name)
        elif isinstance(node, ast.ImportFrom):
            for alias in node.names:
                if alias.name != "*":
                    names.add(alias.asname or alias.name)
        elif isinstance(node, ast.Assign):
            names.update(t.id for t in node.targets if isinstance(t, ast.Name))
        elif isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name):
            names.add(node.target.id)
    return names


def test_stub_file_is_shipped():
    assert STUB.exists(), f"expected public stub at {STUB}"


def test_generated_native_stub_is_valid_python():
    """The generated native stub is shipped alongside the extension."""
    assert CORE_STUB.exists(), f"expected generated native stub at {CORE_STUB}"
    ast.parse(CORE_STUB.read_text())


def test_generated_native_stub_type_checks():
    """Type-check the installed wheel's native API when pyright is available."""
    pyright = shutil.which("pyright")
    if pyright is None:
        return
    subprocess.run([pyright, str(CORE_STUB.parent)], check=True)


def test_stub_declares_every_public_helper():
    missing = sorted(_package_defined_helpers() - _stub_declared_names())
    assert not missing, (
        f"Public helper(s) {missing} are missing from src/rayx/__init__.pyi. "
        "Declare them there (e.g. `from .data import foo as foo` or a `def`) so they "
        "stay in the typed public API."
    )
