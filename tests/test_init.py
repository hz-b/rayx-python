import types
from pathlib import Path

import rayx


def test_get_info_keys():
    info = rayx.get_info()
    assert set(info.keys()) == {"version", "python_wrapper", "cpp_module", "module_path"}

def test_get_info_version_matches():
    info = rayx.get_info()
    assert info["version"] == rayx.__version__

def test_get_info_python_wrapper_is_true():
    info = rayx.get_info()
    assert info["python_wrapper"] is True

def test_get_info_cpp_module_is_so():
    info = rayx.get_info()
    assert info["cpp_module"].endswith(".so")


def test_native_api_exposed_at_top_level():
    # The native bindings live in the private `_core` extension and are re-exported
    # at the top level; there is no separate public `rayx.core` module.
    assert rayx._core.__name__ == "rayx._core"
    assert rayx.Beamline is rayx._core.Beamline
    assert rayx.import_beamline is rayx._core.import_beamline
    assert not hasattr(rayx, "core")


def test_opaque_crystal_type_uses_an_int():
    crystal = Path(__file__).parents[1] / "extern/rayx/Intern/rayx-core/tests/input/crystal.rml"
    beamline = rayx.import_beamline(str(crystal))
    element = beamline.elements[0]
    assert isinstance(element.crystalType, int)
    element.crystalType = element.crystalType


def test_central_beamstop_uses_a_valid_python_member_name():
    assert rayx.CentralBeamstop.NONE.value == 0


def test_star_export_advertises_native_api_and_helpers():
    ns = {}
    exec("from rayx import *", ns)
    for name in ("Beamline", "import_beamline", "list_devices", "rays_to_df", "get_info"):
        assert name in ns, f"{name} missing from `from rayx import *`"

def test_get_info_module_path_exists():
    info = rayx.get_info()
    assert Path(info["module_path"]).exists()


def _package_defined_helpers():
    """Public names the `rayx` package itself defines (Python helpers), excluding
    the native `_core` re-exports (which `__all__` picks up dynamically) and submodules.

    A public helper is a non-underscore attribute whose `__module__` is `rayx` or a
    `rayx.*` submodule other than the private `rayx._core` extension. Internal helpers
    should be underscore-prefixed so they are intentionally excluded.
    """
    helpers = {}
    for name, value in vars(rayx).items():
        if name.startswith("_") or isinstance(value, types.ModuleType):
            continue
        module = getattr(value, "__module__", "") or ""
        if (module == "rayx" or module.startswith("rayx.")) and not module.startswith("rayx._core"):
            helpers[name] = value
    return helpers


def test_all_lists_every_public_helper():
    # Guardrail: adding a public helper (e.g. in data.py) without also adding it to
    # `__init__.__all__` fails here, so `from rayx import *` never silently drops it.
    missing = sorted(set(_package_defined_helpers()) - set(rayx.__all__))
    assert not missing, (
        f"Public helper(s) {missing} are not in rayx.__all__. "
        "Add them to __all__ in src/rayx/__init__.py (or prefix with '_' if internal)."
    )


def test_all_has_no_stale_entries():
    # Guardrail: every name in `__all__` must actually resolve on the package.
    stale = sorted(name for name in rayx.__all__ if not hasattr(rayx, name))
    assert not stale, f"rayx.__all__ lists names that don't exist: {stale}"
