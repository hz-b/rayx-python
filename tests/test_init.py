# tests/test_init.py
import pytest
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

def test_get_info_module_path_exists():
    info = rayx.get_info()
    assert Path(info["module_path"]).exists()
